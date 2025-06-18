#include "iris/geometry.h"

#include <optional>
#include <variant>

#include "iris/emissive_material.h"
#include "iris/float.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/internal/hit_arena.h"
#include "iris/material.h"
#include "iris/point.h"
#include "iris/sampler.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {

Hit* Geometry::Trace(HitAllocator& hit_allocator) const {
  const Geometry* old = hit_allocator.arena_.GetGeometry();

  hit_allocator.arena_.SetGeometry(this);
  Hit* result = Trace(hit_allocator.ray_, hit_allocator);
  hit_allocator.arena_.SetGeometry(old);

  return result;
}

std::optional<TextureCoordinates> Geometry::ComputeTextureCoordinates(
    const Point& hit_point, const std::optional<Differentials>& differentials,
    face_t face, const void* additional_data) const {
  return std::nullopt;
}

Geometry::ComputeShadingNormalResult Geometry::ComputeShadingNormal(
    face_t face, const void* additional_data) const {
  return {std::nullopt, std::nullopt, nullptr};
}

const Material* Geometry::GetMaterial(face_t face) const { return nullptr; }

const EmissiveMaterial* Geometry::GetEmissiveMaterial(face_t face) const {
  return nullptr;
}

std::variant<std::monostate, Point, Vector> Geometry::SampleBySolidAngle(
    const Point& origin, face_t face, Sampler& sampler) const {
  return std::variant<std::monostate, Point, Vector>();
}

std::optional<visual_t> Geometry::ComputePdfBySolidAngle(
    const Point& origin, face_t face, const void* additional_data,
    const Point& on_face) const {
  return std::nullopt;
}

}  // namespace iris
