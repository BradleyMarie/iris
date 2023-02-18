#include "iris/scene_objects.h"

#include <limits>
#include <map>
#include <set>

#include "absl/log/check.h"
#include "iris/internal/area_light.h"
#include "iris/internal/environmental_light.h"

namespace iris {
namespace {

template <typename T>
void ReorderImpl(std::vector<T>& values,
                 std::span<const size_t> new_positions) {
  std::multimap<size_t, T> sorted_values;
  for (size_t i = 0; i < values.size(); i++) {
    if (i < new_positions.size()) {
      sorted_values.insert({new_positions[i], values[i]});
    } else {
      sorted_values.insert({std::numeric_limits<size_t>::max(), values[i]});
    }
  }

  values.clear();
  for (const auto& [key, value] : sorted_values) {
    values.push_back(value);
  }
}

}  // namespace

void SceneObjects::Builder::Add(ReferenceCounted<Geometry> geometry,
                                const Matrix& matrix) {
  if (!geometry || geometry->ComputeBounds(matrix).Empty()) {
    return;
  }

  auto insertion_result = matrix_to_transform_index_.insert(
      {matrix, matrix_to_transform_index_.size()});
  if (insertion_result.second) {
    matrices_.push_back(matrix);
  }

  CHECK(insertion_result.first->second <=
        std::numeric_limits<uintptr_t>::max());
  uintptr_t matrix_index =
      static_cast<uintptr_t>(insertion_result.first->second);

  geometry_.push_back(
      std::make_pair(geometry, reinterpret_cast<const Matrix*>(matrix_index)));
}

void SceneObjects::Builder::Add(ReferenceCounted<Light> light) {
  if (!light) {
    return;
  }

  lights_.push_back(std::move(light));
}

void SceneObjects::Builder::Set(
    ReferenceCounted<EnvironmentalLight> environmental_light) {
  environmental_light_ = std::move(environmental_light);
}

SceneObjects SceneObjects::Builder::Build() {
  assert((std::set<std::pair<ReferenceCounted<Geometry>, const Matrix*>>(
              geometry_.begin(), geometry_.end())
              .size() == geometry_.size()));

  SceneObjects result(std::move(*this));

  matrix_to_transform_index_.clear();
  matrices_.clear();
  geometry_.clear();
  lights_.clear();
  environmental_light_.Reset();

  matrix_to_transform_index_.insert({Matrix::Identity(), 0});
  matrices_.push_back(Matrix::Identity());

  return result;
}

SceneObjects::SceneObjects(Builder&& builder)
    : geometry_(std::move(builder.geometry_)),
      lights_(std::move(builder.lights_)),
      environmental_light_(std::move(builder.environmental_light_)),
      matrices_(std::move(builder.matrices_)) {
  matrices_.shrink_to_fit();
  geometry_.shrink_to_fit();

  for (auto& entry : geometry_) {
    if (entry.second) {
      entry.second = &matrices_.at(reinterpret_cast<uintptr_t>(entry.second));
    }

    for (face_t face : entry.first->GetFaces()) {
      if (!entry.first->IsEmissive(face)) {
        continue;
      }

      lights_.push_back(iris::MakeReferenceCounted<internal::AreaLight>(
          std::cref(*entry.first), entry.second, face));
    }
  }

  if (environmental_light_) {
    lights_.push_back(iris::MakeReferenceCounted<internal::EnvironmentalLight>(
        std::cref(*environmental_light_)));
  }

  lights_.shrink_to_fit();
}

void SceneObjects::Reorder(
    std::span<const size_t> new_geometry_positions,
    std::span<const size_t> new_light_positions) noexcept {
  ReorderImpl(geometry_, new_geometry_positions);
  ReorderImpl(lights_, new_light_positions);
}

}  // namespace iris