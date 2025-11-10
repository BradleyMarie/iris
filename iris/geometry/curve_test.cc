#include "iris/geometry/curve.h"

#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/material.h"
#include "iris/materials/mock_material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/normal_maps/mock_normal_map.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/testing/hit_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {
namespace {

using ::iris::materials::MockMaterial;
using ::iris::normal_maps::MockNormalMap;
using ::iris::testing::AdditionalData;
using ::iris::testing::BackFace;
using ::iris::testing::FrontFace;
using ::iris::testing::MakeHitAllocator;

struct Data {
  Vector direction;
  Vector dp_du;
  Vector dp_dv;
  geometric_t error;
  geometric_t u;
  geometric_t v;
};

static const face_t FRONT_FACE = 0u;

static const ReferenceCounted<Material> front_material =
    MakeReferenceCounted<MockMaterial>();
static const ReferenceCounted<NormalMap> front_normal_map =
    MakeReferenceCounted<MockNormalMap>();

ReferenceCounted<Geometry> MakeCubicBezierCurve() {
  return MakeFlatCubicBezierCurve({Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                                   Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)},
                                  1, 0.25, 0.5, 0.0, 1.0, front_material,
                                  front_normal_map)
      .front();
}

TEST(CubicBezierCurve, Null) {
  EXPECT_TRUE(MakeFlatCubicBezierCurve(
                  {Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0),
                   Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0)},
                  1, 0.1, 0.2, 0.0, 1.0, front_material, front_normal_map)
                  .empty());
  EXPECT_TRUE(MakeFlatCubicBezierCurve(
                  {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                   Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)},
                  1, -0.1, -0.2, 0.0, 1.0, front_material, front_normal_map)
                  .empty());
  EXPECT_TRUE(MakeFlatCubicBezierCurve(
                  {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                   Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)},
                  0, 0.5, 0.6, 0.0, 1.0, front_material, front_normal_map)
                  .empty());
}

TEST(Curve, MissesCompletely) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();

  Point origin(0.0, 0.0, 100.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = curve->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Curve, Hits) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();

  Point origin(0.1, 0.1, 6.0);
  Vector direction(0.0, 0.0, -1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = curve->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(6.0, hit->distance);

  EXPECT_EQ(FRONT_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));

  const Data* data = static_cast<const Data*>(AdditionalData(*hit));
  ASSERT_TRUE(data);
  EXPECT_EQ(direction, data->direction);
  EXPECT_NEAR(3.0000, data->dp_du.x, 0.001);
  EXPECT_NEAR(2.5721, data->dp_du.y, 0.001);
  EXPECT_NEAR(0.0000, data->dp_du.z, 0.001);
  EXPECT_NEAR(0.3374, data->dp_dv.x, 0.001);
  EXPECT_NEAR(-0.3936, data->dp_dv.y, 0.001);
  EXPECT_NEAR(0.0000, data->dp_dv.z, 0.001);
}

TEST(Curve, HitsCylinder) {
  ReferenceCounted<Geometry> curve =
      MakeCylindricalCubicBezierCurve(
          {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0), Point(2.0, 0.0, 0.0),
           Point(3.0, 1.0, 0.0)},
          1, 0.25, 0.5, 0.0, 1.0, front_material, front_normal_map)
          .front();

  Point origin(0.1, 0.1, 6.0);
  Vector direction(0.0, 0.0, -1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = curve->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(6.0, hit->distance);

  EXPECT_EQ(FRONT_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));

  const Data* data = static_cast<const Data*>(AdditionalData(*hit));
  ASSERT_TRUE(data);
  EXPECT_EQ(direction, data->direction);
  EXPECT_NEAR(3.0000, data->dp_du.x, 0.001);
  EXPECT_NEAR(2.5721, data->dp_du.y, 0.001);
  EXPECT_NEAR(0.0000, data->dp_du.z, 0.001);
  EXPECT_NEAR(0.3342, data->dp_dv.x, 0.001);
  EXPECT_NEAR(-0.3898, data->dp_dv.y, 0.001);
  EXPECT_NEAR(-0.0719, data->dp_dv.z, 0.001);
}

TEST(Curve, ComputeSurfaceNormal) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();

  Data data{
      Vector(0.0, 0.0, 2.0),
      Vector(1.0, 0.0, 0.0),
      Vector(0.0, 1.0, 0.0),
  };

  Vector normal =
      curve->ComputeSurfaceNormal(Point(0.0, 0.0, 0.0), FRONT_FACE, &data);
  EXPECT_EQ(Vector(0.0, 0.0, -1.0), normal);
}

TEST(Curve, ComputeTextureCoordinatesNone) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();

  Data data{
      Vector(0.0, 0.0, 3.0),
      Vector(1.0, 0.0, 0.0),
      Vector(0.0, 1.0, 0.0),
      0.0,
      0.125,
      0.250,
  };

  std::optional<Geometry::TextureCoordinates> texture_coordinates =
      curve->ComputeTextureCoordinates(Point(0.0, 0.0, 0.0), std::nullopt,
                                       FRONT_FACE, &data);
  ASSERT_TRUE(texture_coordinates);
  EXPECT_EQ(texture_coordinates->face_index, FRONT_FACE);
  EXPECT_EQ(texture_coordinates->uv[0], 0.125);
  EXPECT_EQ(texture_coordinates->uv[1], 0.250);
}

TEST(Curve, ComputeShadingNormal) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();

  Data data{
      Vector(0.0, 0.0, 3.0),
      Vector(1.0, 0.0, 0.0),
      Vector(0.0, 1.0, 0.0),
      0.0,
      0.125,
      0.250,
  };

  Geometry::ComputeShadingNormalResult front_normal =
      curve->ComputeShadingNormal(FRONT_FACE, &data);
  ASSERT_FALSE(front_normal.surface_normal);
  ASSERT_TRUE(front_normal.dp_duv);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), front_normal.dp_duv->first);
  EXPECT_EQ(Vector(0.0, 1.0, 0.0), front_normal.dp_duv->second);
  EXPECT_EQ(front_normal_map.Get(), front_normal.normal_map);
}

TEST(Curve, ComputeHitPoint) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();

  Data data{
      Vector(0.0, 0.0, 3.0),
      Vector(1.0, 0.0, 0.0),
      Vector(0.0, 1.0, 0.0),
      1.0,
  };

  Point origin(0.0, 0.0, 3.0);
  Vector direction0(0.0, 1.0, 0.0);
  Ray ray0(origin, direction0);

  Geometry::ComputeHitPointResult hit_point0 =
      curve->ComputeHitPoint(ray0, 2.0, &data);
  Point expected_hit_point0 = ray0.Endpoint(2.0);
  EXPECT_NEAR(hit_point0.point.x, expected_hit_point0.x, 0.00001);
  EXPECT_NEAR(hit_point0.point.y, expected_hit_point0.y, 0.00001);
  EXPECT_NEAR(hit_point0.point.z, expected_hit_point0.z, 0.00001);
  EXPECT_LE(hit_point0.error.x, 1.0);
  EXPECT_LE(hit_point0.error.y, 1.0);
  EXPECT_LE(hit_point0.error.z, 1.0);
}

TEST(Curve, GetMaterial) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();

  const Material* front = curve->GetMaterial(FRONT_FACE);
  EXPECT_EQ(front_material.Get(), front);
}

TEST(Curve, GetBounds) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();
  BoundingBox bounds = curve->ComputeBounds(nullptr);
  EXPECT_EQ(Point(-0.5, -0.5, -0.5), bounds.lower);
  EXPECT_EQ(Point(3.5, 1.5, 0.5), bounds.upper);
}

TEST(Curve, GetBoundsWithTransform) {
  ReferenceCounted<Geometry> curve = MakeCubicBezierCurve();
  Matrix transform = Matrix::Translation(1.0, 2.0, 3.0).value();
  BoundingBox bounds = curve->ComputeBounds(&transform);
  EXPECT_EQ(Point(0.5, 1.5, 2.5), bounds.lower);
  EXPECT_EQ(Point(4.5, 3.5, 3.5), bounds.upper);
}

}  // namespace
}  // namespace geometry
}  // namespace iris
