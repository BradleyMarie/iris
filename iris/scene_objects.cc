#include "iris/scene_objects.h"

#include <ranges>

#include "iris/internal/area_light.h"
#include "iris/internal/environmental_light.h"

namespace iris {
namespace {

template <typename T>
std::vector<T> ToVector(std::map<T, size_t> values) {
  std::vector<T> result(values.size());
  for (auto& [value, index] : values) {
    result[index] = std::move(value);
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

  ordered_geometry_.try_emplace({std::move(geometry), model_to_world},
                                ordered_geometry_.size());
}

void SceneObjects::Builder::Add(ReferenceCounted<Light> light) {
  if (!light) {
    return;
  }

  ordered_lights_.try_emplace(std::move(light), ordered_lights_.size());
}

void SceneObjects::Builder::Set(
    ReferenceCounted<EnvironmentalLight> environmental_light) {
  environmental_light_ = std::move(environmental_light);
}

SceneObjects SceneObjects::Builder::Build() {
  for (auto& [entry, _] : ordered_geometry_) {
    for (face_t face : entry.first->GetFaces()) {
      if (!entry.first->GetEmissiveMaterial(face)) {
        continue;
      }

      ordered_lights_.try_emplace(
          MakeReferenceCounted<internal::AreaLight>(std::cref(*entry.first),
                                                    entry.second, face),
          ordered_lights_.size());
    }
  }

  if (environmental_light_) {
    ordered_lights_.try_emplace(
        MakeReferenceCounted<internal::EnvironmentalLight>(
            std::cref(*environmental_light_)),
        ordered_lights_.size());
  }

  SceneObjects result(ToVector(std::move(ordered_geometry_)),
                      ToVector(std::move(ordered_lights_)),
                      std::move(matrices_), std::move(environmental_light_),
                      bounds_builder_);
  ordered_geometry_.clear();
  ordered_lights_.clear();
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
