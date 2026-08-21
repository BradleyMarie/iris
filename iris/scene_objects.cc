#include "iris/scene_objects.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "iris/bounding_box.h"
#include "iris/environmental_light.h"
#include "iris/geometry.h"
#include "iris/internal/area_light.h"
#include "iris/internal/environmental_light.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris {
namespace {

using ::iris::internal::MakeAreaLight;
using ::iris::internal::MakeEnvironmentalLight;

template <typename T>
void ReorderImpl(std::vector<T>& values, std::span<const size_t> new_order) {
  std::vector<std::pair<size_t, size_t>> zipped;
  zipped.reserve(new_order.size());
  for (size_t i = 0; i < new_order.size(); ++i) {
    zipped.emplace_back(new_order[i], i);
  }

  std::sort(zipped.begin(), zipped.end(),
            [](const auto& left, const auto& right) {
              return left.first < right.first;
            });

  std::vector<T> moved_values;
  moved_values.reserve(new_order.size());
  for (size_t i = 0; i < new_order.size(); i++) {
    moved_values.push_back(std::move(values[i]));
  }

  for (size_t i = 0; i < new_order.size(); i++) {
    values[i] = std::move(moved_values[zipped[i].second]);
  }
}

const Matrix* ToNullableMatrix(const Matrix& matrix) {
  return (matrix == Matrix::Identity()) ? nullptr : &matrix;
}

void Deduplicate(
    const std::deque<Matrix>& all_matrices,
    std::vector<std::pair<ReferenceCounted<Geometry>, const Matrix*>>&
        visible_geometry,
    std::vector<std::pair<ReferenceCounted<Geometry>, const Matrix*>>&
        invisible_geometry,
    std::vector<ReferenceCounted<Light>>& all_lights) {
  absl::flat_hash_set<std::reference_wrapper<const Matrix>> matrices(
      all_matrices.begin(), all_matrices.end());

  absl::flat_hash_set<std::pair<const Geometry*, const Matrix*>> geometry;
  geometry.reserve(visible_geometry.size());
  auto predicate =
      [&](std::pair<ReferenceCounted<Geometry>, const Matrix*>& entry) {
        if (entry.second != nullptr) {
          auto iter = matrices.find(*entry.second);
          if (iter != matrices.end()) {
            entry.second = &iter->get();
          }
        }

        auto [_, inserted] = geometry.emplace(entry.first.Get(), entry.second);
        return !inserted;
      };

  std::erase_if(visible_geometry, predicate);
  std::erase_if(invisible_geometry, predicate);
}

void AddAreaLights(
    const std::pair<ReferenceCounted<Geometry>, const Matrix*>& geometry,
    bool invisible,
    absl::flat_hash_set<std::pair<const Geometry*, const Matrix*>>& emissive,
    std::vector<ReferenceCounted<Light>>& lights) {
  size_t index = 0;
  std::span<const face_t> faces = geometry.first->GetFaces();

  const EmissiveMaterial* emissive_material = nullptr;
  for (; index < faces.size(); index++) {
    emissive_material = geometry.first->GetEmissiveMaterial(faces[index]);
    if (emissive_material != nullptr) {
      break;
    }
  }

  if (emissive_material == nullptr) {
    return;
  }

  auto [_, inserted] = emissive.emplace(geometry.first.Get(), geometry.second);
  if (!inserted) {
    return;
  }

  lights.push_back(MakeAreaLight(geometry.first, geometry.second,
                                 faces[index++], invisible));

  for (; index < faces.size(); index++) {
    emissive_material = geometry.first->GetEmissiveMaterial(faces[index]);
    if (emissive_material != nullptr) {
      lights.push_back(MakeAreaLight(geometry.first, geometry.second,
                                     faces[index], invisible));
    }
  }
}

void ProcessGeometry(
    const std::vector<std::pair<ReferenceCounted<Geometry>, const Matrix*>>&
        geometry,
    bool invisible,
    absl::flat_hash_set<std::pair<const Geometry*, const Matrix*>>& emissive,
    std::vector<ReferenceCounted<Light>>& lights) {
  for (const auto& entry : geometry) {
    AddAreaLights(entry, invisible, emissive, lights);
  }
}

}  // namespace

void SceneObjects::Builder::Add(ReferenceCounted<Geometry> geometry,
                                const Matrix& matrix, bool invisible) {
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
    model_to_world = &matrices_.emplace_back(*model_to_world);
  }

  if (invisible) {
    invisible_geometry_.emplace_back(std::move(geometry), model_to_world);
  } else {
    visible_geometry_.emplace_back(std::move(geometry), model_to_world);
  }
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

SceneObjects SceneObjects::Builder::Build(bool deduplicate) {
  if (deduplicate) {
    Deduplicate(matrices_, visible_geometry_, invisible_geometry_, lights_);
  }

  // Lights are always deduplicated to avoid potential artifacts
  absl::flat_hash_set<const Light*> lights;
  std::erase_if(lights_, [&](const ReferenceCounted<Light>& light) {
    auto [_, inserted] = lights.emplace(light.Get());
    return !inserted;
  });

  absl::flat_hash_set<std::pair<const Geometry*, const Matrix*>> emissive;
  ProcessGeometry(visible_geometry_, /*invisible=*/false, emissive, lights_);
  ProcessGeometry(invisible_geometry_, /*invisible=*/true, emissive, lights_);

  if (environmental_light_) {
    lights_.push_back(MakeEnvironmentalLight(std::cref(*environmental_light_)));
  }

  SceneObjects result(std::move(visible_geometry_), std::move(lights_),
                      std::move(matrices_), std::move(environmental_light_),
                      bounds_builder_);
  matrices_.clear();
  visible_geometry_.clear();
  invisible_geometry_.clear();
  lights_.clear();
  environmental_light_.Reset();
  bounds_builder_.Reset();
  return result;
}

#ifndef NDEBUG

std::pair<const Geometry&, const Matrix*> SceneObjects::GetGeometry(
    size_t index) const noexcept {
  assert(index < geometry_.size());
  return std::pair<const Geometry&, const Matrix*>(*geometry_[index].first,
                                                   geometry_[index].second);
}

const Light& SceneObjects::GetLight(size_t index) const noexcept {
  assert(index < lights_.size());
  return *lights_[index];
}

#endif  // NDEBUG

void SceneObjects::Reorder(std::span<const size_t> new_geometry_order,
                           std::span<const size_t> new_light_order) noexcept {
  ReorderImpl(geometry_, new_geometry_order);
  ReorderImpl(lights_, new_light_order);
}

}  // namespace iris
