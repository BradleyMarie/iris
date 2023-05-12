#ifndef _IRIS_GEOMETRY_MOCK_GEOMETRY_
#define _IRIS_GEOMETRY_MOCK_GEOMETRY_

#include "googlemock/include/gmock/gmock.h"
#include "iris/geometry.h"

namespace iris {
namespace geometry {

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
              (const Point&, face_t, const void*), (const override));
  MOCK_METHOD((std::variant<Vector, const NormalMap*>), ComputeShadingNormal,
              (face_t, const void*), (const override));
  MOCK_METHOD(const Material*, GetMaterial, (face_t, const void*),
              (const override));
  MOCK_METHOD(bool, IsEmissive, (face_t), (const override));
  MOCK_METHOD(const EmissiveMaterial*, GetEmissiveMaterial,
              (face_t, const void*), (const override));
  MOCK_METHOD(std::optional<Point>, SampleByArea, (face_t, Sampler&),
              (const override));
  MOCK_METHOD(std::optional<Geometry::SampleBySolidAngleResult>,
              SampleBySolidAngle, (const Point&, face_t, Sampler& sampler),
              (const override));
  MOCK_METHOD(std::optional<visual_t>, ComputeArea, (face_t), (const override));
};

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_MOCK_GEOMETRY_