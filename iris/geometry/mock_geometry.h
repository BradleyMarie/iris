#ifndef _IRIS_GEOMETRY_MOCK_GEOMETRY_
#define _IRIS_GEOMETRY_MOCK_GEOMETRY_

#include "googlemock/include/gmock/gmock.h"
#include "iris/geometry.h"

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
  MOCK_METHOD(Hit*, Trace, (const Ray&, HitAllocator&), (const override));
  MOCK_METHOD(Vector, ComputeSurfaceNormal, (const Point&, face_t, const void*),
              (const override));
  MOCK_METHOD(BoundingBox, ComputeBounds, (const Matrix&), (const override));
  MOCK_METHOD(std::span<const face_t>, GetFaces, (), (const override));
};

class MockGeometry : public MockBasicGeometry {
 public:
  MOCK_METHOD(std::optional<TextureCoordinates>, ComputeTextureCoordinates,
              (const Point&, const std::optional<Differentials>&, face_t,
               const void*),
              (const override));
  MOCK_METHOD((std::variant<Vector, const NormalMap*>), ComputeShadingNormal,
              (face_t, const void*), (const override));
  MOCK_METHOD(const Material*, GetMaterial, (face_t, const void*),
              (const override));
  MOCK_METHOD(bool, IsEmissive, (face_t), (const override));
  MOCK_METHOD(const EmissiveMaterial*, GetEmissiveMaterial,
              (face_t, const void*), (const override));
  MOCK_METHOD((std::variant<std::monostate, Point, Vector>), SampleBySolidAngle,
              (const Point&, face_t, Sampler&), (const override));
  MOCK_METHOD(std::optional<visual_t>, ComputePdfBySolidAngle,
              (const Point&, face_t, const Point&), (const override));
};

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_MOCK_GEOMETRY_