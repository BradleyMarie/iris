#ifndef _IRIS_GEOMETRY_
#define _IRIS_GEOMETRY_

#include <optional>
#include <span>
#include <utility>
#include <variant>

#include "iris/bounding_box.h"
#include "iris/emissive_material.h"
#include "iris/float.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/reference_countable.h"
#include "iris/sampler.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {

class Geometry : public ReferenceCountable {
 public:
  Hit* TraceAllHits(HitAllocator& hit_allocator) const;

  Hit* TraceOneHit(HitAllocator& hit_allocator, geometric_t minimum_distance,
                   geometric_t maximum_distance, bool find_closest_hit) const;

  virtual Vector ComputeSurfaceNormal(const Point& hit_point, face_t face,
                                      const void* additional_data) const = 0;

  struct Differentials {
    const Point dx;
    const Point dy;
  };

  virtual std::optional<TextureCoordinates> ComputeTextureCoordinates(
      const Point& hit_point, const std::optional<Differentials>& differentials,
      face_t face, const void* additional_data) const;

  struct ComputeShadingNormalResult {
    std::optional<Vector> surface_normal;
    std::optional<std::pair<Vector, Vector>> dp_duv;
    const NormalMap* normal_map;
  };

  virtual ComputeShadingNormalResult ComputeShadingNormal(
      face_t face, const void* additional_data) const;

  struct ComputeHitPointResult {
    const Point point;
    const PositionError error;
  };

  virtual ComputeHitPointResult ComputeHitPoint(
      const Ray& ray, geometric_t distance,
      const void* additional_data) const = 0;

  virtual const Material* GetMaterial(face_t face) const;

  virtual const EmissiveMaterial* GetEmissiveMaterial(face_t face) const;

  virtual visual_t ComputeSurfaceArea(face_t face,
                                      const Matrix* model_to_world) const = 0;

  virtual std::variant<std::monostate, Point, Vector> SampleBySolidAngle(
      const Point& origin, face_t face, Sampler& sampler) const;

  virtual std::optional<visual_t> ComputePdfBySolidAngle(
      const Point& origin, face_t face, const void* additional_data,
      const Point& on_face) const;

  virtual BoundingBox ComputeBounds(const Matrix* model_to_world) const = 0;

  virtual std::span<const face_t> GetFaces() const = 0;

  virtual ~Geometry() = default;

 private:
  virtual Hit* Trace(const Ray& ray, HitAllocator& hit_allocator) const = 0;
};

}  // namespace iris

#endif  // _IRIS_GEOMETRY_
