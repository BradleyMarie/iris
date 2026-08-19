#include "iris/scene_objects.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <utility>
#include <vector>

#include "absl/hash/hash.h"
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

std::size_t SceneObjects::Builder::MatrixPtrHash::operator()(
    const Matrix* ptr) const {
  return absl::Hash<std::array<std::array<geometric, 4>, 4>>()(ptr->m);
}

bool SceneObjects::Builder::MatrixPtrEqual::operator()(
    const Matrix* lhs, const Matrix* rhs) const {
  return *lhs == *rhs;
}

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
    matrix_storage_.push_back(*model_to_world);

    auto [iter, inserted] = matrices_.insert(&matrix_storage_.back());
    if (!inserted) {
      matrix_storage_.pop_back();
    }

    model_to_world = *iter;
  }

  auto [iter, inserted] =
      geometry_.try_emplace({geometry.Get(), model_to_world}, nullptr);
  if (inserted) {
    geometry_storage_.emplace_back(std::move(geometry), model_to_world,
                                   invisible);
    iter->second = &std::get<2>(geometry_storage_.back());
  }

  *iter->second &= invisible;
}

void SceneObjects::Builder::Add(ReferenceCounted<Light> light) {
  if (!light) {
    return;
  }

  auto [_, inserted] = lights_.insert(light.Get());
  if (inserted) {
    light_storage_.push_back(std::move(light));
  }
}

void SceneObjects::Builder::Set(
    ReferenceCounted<EnvironmentalLight> environmental_light) {
  environmental_light_ = std::move(environmental_light);
}

SceneObjects SceneObjects::Builder::Build() {
  std::vector<std::pair<ReferenceCounted<Geometry>, const Matrix*>>
      geometry_only;
  geometry_only.reserve(geometry_storage_.size());

  for (auto& [geometry, model_to_world, invisible] : geometry_storage_) {
    for (face_t face : geometry->GetFaces()) {
      if (!geometry->GetEmissiveMaterial(face)) {
        continue;
      }

      light_storage_.push_back(
          MakeAreaLight(geometry, model_to_world, face, invisible));
    }

    if (!invisible) {
      geometry_only.emplace_back(std::move(geometry), model_to_world);
    }
  }

  if (environmental_light_) {
    light_storage_.push_back(
        MakeEnvironmentalLight(std::cref(*environmental_light_)));
  }

  SceneObjects result(std::move(geometry_only), std::move(light_storage_),
                      std::move(matrix_storage_),
                      std::move(environmental_light_), bounds_builder_);
  matrices_.clear();
  matrix_storage_.clear();
  geometry_.clear();
  geometry_storage_.clear();
  lights_.clear();
  light_storage_.clear();
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

void SceneObjects::Reorder(std::vector<size_t> new_geometry_positions,
                           std::vector<size_t> new_light_positions) noexcept {
  ReorderImpl(geometry_, new_geometry_positions);
  ReorderImpl(lights_, new_light_positions);
}

}  // namespace iris
