#include "iris/geometry.h"

#include <limits>
#include <optional>
#include <variant>

#include "iris/emissive_material.h"
#include "iris/float.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/internal/hit.h"
#include "iris/internal/hit_arena.h"
#include "iris/material.h"
#include "iris/point.h"
#include "iris/sampler.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {

Hit* Geometry::TraceAllHits(HitAllocator& hit_allocator) const {
  const Geometry* old = hit_allocator.arena_.GetGeometry();

  hit_allocator.arena_.SetGeometry(this);
  Hit* result = Trace(
      hit_allocator.ray_, -std::numeric_limits<geometric_t>::infinity(),
      std::numeric_limits<geometric_t>::infinity(), ALL_HITS, hit_allocator);
  hit_allocator.arena_.SetGeometry(old);

  return result;
}

Hit* Geometry::TraceOneHit(HitAllocator& hit_allocator,
                           geometric_t minimum_distance,
                           geometric_t maximum_distance,
                           bool find_closest_hit) const {
  const Geometry* old = hit_allocator.arena_.GetGeometry();

  hit_allocator.arena_.SetGeometry(this);
  Hit* hit_list =
      Trace(hit_allocator.ray_, minimum_distance, maximum_distance,
            find_closest_hit ? CLOSEST_HIT : ANY_HIT, hit_allocator);
  hit_allocator.arena_.SetGeometry(old);

  Hit* closest = nullptr;
  for (Hit* current = hit_list; current; current = current->next) {
    internal::Hit* full_hit = static_cast<internal::Hit*>(current);

    if (current->distance - full_hit->error <= minimum_distance ||
        current->distance >= maximum_distance) {
      continue;
    }

    if (!find_closest_hit) {
      current->next = nullptr;
      return current;
    }

    if (!closest || current->distance < closest->distance) {
      closest = current;
    }
  }

  if (closest) {
    closest->next = nullptr;
  }

  return closest;
}

std::optional<Geometry::TextureCoordinates> Geometry::ComputeTextureCoordinates(
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
