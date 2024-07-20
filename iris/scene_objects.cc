#include "iris/scene_objects.h"

#include <map>
#include <unordered_map>

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
      sorted_values.emplace(new_positions[i], std::move(values[i]));
    } else {
      sorted_values.emplace(std::numeric_limits<size_t>::max(),
                            std::move(values[i]));
    }
  }

  size_t i = 0;
  for (auto& [key, value] : sorted_values) {
    values[i++] = std::move(value);
  }
}

const Matrix* ToNullableMatrix(const Matrix& matrix) {
  return (matrix == Matrix::Identity()) ? nullptr : &matrix;
}

}  // namespace

void SceneObjects::Builder::Add(ReferenceCounted<Geometry> geometry,
                                const Matrix& matrix) {
  const Matrix* model_to_world = ToNullableMatrix(matrix);
  if (!geometry || geometry->ComputeBounds(model_to_world).Empty()) {
    return;
  }

  if (model_to_world != nullptr) {
    model_to_world = &*matrices_.insert(matrix).first;
  }

  geometry_.emplace(std::move(geometry), model_to_world);
}

void SceneObjects::Builder::Add(ReferenceCounted<Light> light) {
  if (!light) {
    return;
  }

  lights_.insert(std::move(light));
}

void SceneObjects::Builder::Set(
    ReferenceCounted<EnvironmentalLight> environmental_light) {
  environmental_light_ = std::move(environmental_light);
}

SceneObjects SceneObjects::Builder::Build() {
  SceneObjects result(std::move(*this));

  geometry_.clear();
  lights_.clear();
  matrices_.clear();
  environmental_light_.Reset();

  return result;
}

SceneObjects::SceneObjects(Builder&& builder)
    : geometry_(std::move_iterator(builder.geometry_.begin()),
                std::move_iterator(builder.geometry_.end())),
      lights_(std::move_iterator(builder.lights_.begin()),
              std::move_iterator(builder.lights_.end())),
      environmental_light_(std::move(builder.environmental_light_)) {
  std::unordered_map<const Matrix*, const Matrix*> moved_matrices;
  moved_matrices.reserve(builder.matrices_.size());

  matrices_.reserve(builder.matrices_.size());  // Required for correctness
  for (auto& entry : geometry_) {
    if (entry.second != nullptr) {
      const Matrix*& new_matrix = moved_matrices[entry.second];
      if (new_matrix == nullptr) {
        matrices_.push_back(*entry.second);
        new_matrix = &matrices_.back();
      }

      entry.second = new_matrix;
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
}

void SceneObjects::Reorder(
    std::span<const size_t> new_geometry_positions,
    std::span<const size_t> new_light_positions) noexcept {
  ReorderImpl(geometry_, new_geometry_positions);
  ReorderImpl(lights_, new_light_positions);
}

}  // namespace iris