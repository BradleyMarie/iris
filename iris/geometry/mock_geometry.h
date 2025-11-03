#ifndef _IRIS_GEOMETRY_MOCK_GEOMETRY_
#define _IRIS_GEOMETRY_MOCK_GEOMETRY_

#include <optional>
#include <span>
#include <variant>

#include "googlemock/include/gmock/gmock.h"
#include "iris/bounding_box.h"
#include "iris/emissive_material.h"
#include "iris/float.h"
#include "iris/geometry.h"
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
namespace geometry {

static inline bool operator==(
    const std::optional<Geometry::Differentials>& d0,
    const std::optional<Geometry::Differentials>& d1) {
  if (!d0 && !d1) {
    return true;
  }

  if (!d0 || !d1) {
    return false;
  }

  return d0->dx == d1->dx && d0->dy == d1->dy;
}

class MockBasicGeometry : public Geometry {
 public:
  MOCK_METHOD(Hit*, Trace,
              (const Ray&, geometric_t, geometric_t, TraceMode, HitAllocator&),
              (const override));
  MOCK_METHOD(ComputeHitPointResult, ComputeHitPoint,
              (const Ray&, geometric_t, const void*), (const override));
  MOCK_METHOD(std::optional<visual_t>, ComputeSurfaceArea,
              (face_t, const Matrix*), (const override));
  MOCK_METHOD(Vector, ComputeSurfaceNormal, (const Point&, face_t, const void*),
              (const override));
  MOCK_METHOD(BoundingBox, ComputeBounds, (const Matrix*), (const override));
  MOCK_METHOD(std::span<const face_t>, GetFaces, (), (const override));
};

class MockGeometry : public MockBasicGeometry {
 public:
  MOCK_METHOD(std::optional<TextureCoordinates>, ComputeTextureCoordinates,
              (const Point&, const std::optional<Differentials>&, face_t,
               const void*),
              (const override));
  MOCK_METHOD((Geometry::ComputeShadingNormalResult), ComputeShadingNormal,
              (face_t, const void*), (const override));
  MOCK_METHOD(const Material*, GetMaterial, (face_t), (const override));
  MOCK_METHOD(const EmissiveMaterial*, GetEmissiveMaterial, (face_t),
              (const override));
  MOCK_METHOD((std::variant<std::monostate, Point, Vector>), SampleBySolidAngle,
              (const Point&, face_t, Sampler&), (const override));
  MOCK_METHOD(std::optional<visual_t>, ComputePdfBySolidAngle,
              (const Point&, face_t, const void*, const Point&),
              (const override));
};

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_MOCK_GEOMETRY_
