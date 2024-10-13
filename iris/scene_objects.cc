#include "iris/scene_objects.h"

#include <ranges>

#include "iris/internal/area_light.h"
#include "iris/internal/environmental_light.h"

namespace iris {
namespace {

template <typename T>
std::vector<T> MoveToVector(std::set<T> values) {
  std::vector<T> result;
  result.reserve(values.size());
  while (!values.empty()) {
    result.push_back(std::move(values.extract(values.begin()).value()));
  }
  return result;
}

template <typename T>
void ReorderImpl(std::vector<T>& values, std::vector<size_t>& new_positions) {
  std::vector<size_t> to_new_position;
  for (size_t i = 0; i < new_positions.size(); i++) {
    to_new_position.emplace_back(i);
  }

  std::ranges::sort(std::views::zip(new_positions, to_new_position),
                    [](auto&& left, auto&& right) {
                      return std::get<0>(left) < std::get<0>(right);
                    });

  std::vector<T> moved_values;
  for (size_t i = 0; i < new_positions.size(); i++) {
    moved_values.push_back(std::move(values[i]));
  }

  for (size_t i = 0; i < new_positions.size(); i++) {
    values[i] = std::move(moved_values[to_new_position[i]]);
  }
}

const Matrix* ToNullableMatrix(const Matrix& matrix) {
  return (matrix == Matrix::Identity()) ? nullptr : &matrix;
}

}  // namespace

void SceneObjects::Builder::Add(ReferenceCounted<Geometry> geometry,
                                const Matrix& matrix) {
  const Matrix* model_to_world = ToNullableMatrix(matrix);
  if (!geometry) {
    return;
  }

  BoundingBox bounds = geometry->ComputeBounds(model_to_world);
  if (bounds.Empty()) {
    return;
  }

  bounds_builder_.Add(bounds);

  if (model_to_world != nullptr) {
    model_to_world = &*matrices_.insert(matrix).first;
  }

  geometry_.emplace(std::move(geometry), std::move(model_to_world));
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
  for (auto& entry : geometry_) {
    for (face_t face : entry.first->GetFaces()) {
      if (!entry.first->GetEmissiveMaterial(face)) {
        continue;
      }

      lights_.insert(iris::MakeReferenceCounted<internal::AreaLight>(
          std::cref(*entry.first), entry.second, face));
    }
  }

  if (environmental_light_) {
    lights_.insert(iris::MakeReferenceCounted<internal::EnvironmentalLight>(
        std::cref(*environmental_light_)));
  }

  SceneObjects result(MoveToVector(std::move(geometry_)),
                      MoveToVector(std::move(lights_)), std::move(matrices_),
                      std::move(environmental_light_), bounds_builder_);
  geometry_.clear();
  lights_.clear();
  matrices_.clear();
  environmental_light_.Reset();
  bounds_builder_.Reset();
  return result;
}

void SceneObjects::Reorder(std::vector<size_t> new_geometry_positions,
                           std::vector<size_t> new_light_positions) noexcept {
  ReorderImpl(geometry_, new_geometry_positions);
  ReorderImpl(lights_, new_light_positions);
}

}  // namespace iris