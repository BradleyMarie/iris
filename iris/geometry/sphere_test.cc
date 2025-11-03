#include "iris/geometry/sphere.h"

#include <array>
#include <numbers>
#include <optional>
#include <span>
#include <variant>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/emissive_material.h"
#include "iris/emissive_materials/mock_emissive_material.h"
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
#include "iris/random/mock_random.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/sampler.h"
#include "iris/testing/hit_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {
namespace {

using ::iris::emissive_materials::MockEmissiveMaterial;
using ::iris::materials::MockMaterial;
using ::iris::normal_maps::MockNormalMap;
using ::iris::random::MockRandom;
using ::iris::testing::BackFace;
using ::iris::testing::FrontFace;
using ::iris::testing::MakeHitAllocator;
using ::testing::InSequence;
using ::testing::Return;

typedef std::array<geometric_t, 3> AdditionalData;
static const face_t FRONT_FACE = 0u;
static const face_t BACK_FACE = 1u;

static const ReferenceCounted<Material> front_material =
    MakeReferenceCounted<MockMaterial>();
static const ReferenceCounted<Material> back_material =
    MakeReferenceCounted<MockMaterial>();
static const ReferenceCounted<EmissiveMaterial> front_emissive_material =
    MakeReferenceCounted<MockEmissiveMaterial>();
static const ReferenceCounted<EmissiveMaterial> back_emissive_material =
    MakeReferenceCounted<MockEmissiveMaterial>();
static const ReferenceCounted<NormalMap> front_normal_map =
    MakeReferenceCounted<MockNormalMap>();
static const ReferenceCounted<NormalMap> back_normal_map =
    MakeReferenceCounted<MockNormalMap>();

ReferenceCounted<Geometry> MakeSimpleSphere() {
  return AllocateSphere(Point(0.0, 0.0, 3.0), 2.0, front_material,
                        back_material, front_emissive_material,
                        back_emissive_material, front_normal_map,
                        back_normal_map);
}

TEST(Sphere, Null) {
  EXPECT_FALSE(AllocateSphere(Point(0.0, 0.0, 3.0), 0.0, front_material,
                              back_material, front_emissive_material,
                              back_emissive_material, front_normal_map,
                              back_normal_map));
}

TEST(Sphere, MissesCompletely) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Point origin(0.0, 100.0, 100.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = sphere->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Sphere, MissesOnEdge) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Point origin(0.0, 2.0, 0.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = sphere->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Sphere, Inside) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Point origin(0.0, 0.0, 2.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = sphere->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(-1.0, hit->distance);

  EXPECT_EQ(BACK_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));

  ASSERT_NE(nullptr, hit->next);
  EXPECT_EQ(3.0, hit->next->distance);

  EXPECT_EQ(BACK_FACE, FrontFace(*hit->next));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit->next));
}

TEST(Sphere, InFront) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Point origin(0.0, 0.0, 0.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = sphere->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);

  EXPECT_EQ(FRONT_FACE, FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, BackFace(*hit));

  ASSERT_NE(nullptr, hit->next);
  EXPECT_EQ(5.0, hit->next->distance);

  EXPECT_EQ(BACK_FACE, FrontFace(*hit->next));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit->next));
}

TEST(Sphere, Behind) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Point origin(0.0, 0.0, 6.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = sphere->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(-5.0, hit->distance);

  EXPECT_EQ(BACK_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));

  ASSERT_NE(nullptr, hit->next);
  EXPECT_EQ(-1.0, hit->next->distance);

  EXPECT_EQ(FRONT_FACE, FrontFace(*hit->next));
  EXPECT_EQ(BACK_FACE, BackFace(*hit->next));
}

TEST(Sphere, ComputeSurfaceNormal) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Vector front_normal =
      sphere->ComputeSurfaceNormal(Point(0.0, 0.0, 0.0), FRONT_FACE, nullptr);
  EXPECT_EQ(Vector(0.0, 0.0, -3.0), front_normal);

  Vector back_normal =
      sphere->ComputeSurfaceNormal(Point(0.0, 0.0, 0.0), BACK_FACE, nullptr);
  EXPECT_EQ(Vector(0.0, 0.0, 3.0), back_normal);
}

TEST(Sphere, ComputeTextureCoordinatesNone) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();
  std::optional<Geometry::TextureCoordinates> texture_coordinates =
      sphere->ComputeTextureCoordinates(Point(0.0, 0.0, 0.0), std::nullopt,
                                        FRONT_FACE, nullptr);
  EXPECT_FALSE(texture_coordinates);
}

TEST(Sphere, ComputeShadingNormal) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Geometry::ComputeShadingNormalResult front_normal =
      sphere->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  EXPECT_FALSE(front_normal.dp_duv);
  EXPECT_EQ(front_normal_map.Get(), front_normal.normal_map);

  Geometry::ComputeShadingNormalResult back_normal =
      sphere->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.surface_normal);
  EXPECT_FALSE(front_normal.dp_duv);
  EXPECT_EQ(back_normal_map.Get(), back_normal.normal_map);
}

TEST(Sphere, ComputeHitPoint) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Point origin(0.0, 0.0, 3.0);
  Vector direction0(0.0, 1.0, 0.0);
  Ray ray0(origin, direction0);

  Geometry::ComputeHitPointResult hit_point0 =
      sphere->ComputeHitPoint(ray0, 2.0, nullptr);
  Point expected_hit_point0 = ray0.Endpoint(2.0);
  EXPECT_NEAR(hit_point0.point.x, expected_hit_point0.x, 0.00001);
  EXPECT_NEAR(hit_point0.point.y, expected_hit_point0.y, 0.00001);
  EXPECT_NEAR(hit_point0.point.z, expected_hit_point0.z, 0.00001);
  EXPECT_LE(std::abs(hit_point0.error.x), 8.8e-08 * RoundingError(5));
  EXPECT_LE(std::abs(hit_point0.error.y), 2.0 * RoundingError(5));
  EXPECT_LE(std::abs(hit_point0.error.z), 8.8e-08 * RoundingError(3));

  Vector direction1(0.0, -1.0, 0.0);
  Ray ray1(origin, direction1);

  Geometry::ComputeHitPointResult hit_point1 =
      sphere->ComputeHitPoint(ray1, 2.0, nullptr);
  Point expected_hit_point1 = ray1.Endpoint(2.0);
  EXPECT_NEAR(hit_point1.point.x, expected_hit_point1.x, 0.00001);
  EXPECT_NEAR(hit_point1.point.y, expected_hit_point1.y, 0.00001);
  EXPECT_NEAR(hit_point1.point.z, expected_hit_point1.z, 0.00001);
  EXPECT_LE(std::abs(hit_point0.error.x), 8.8e-08 * RoundingError(5));
  EXPECT_LE(std::abs(hit_point0.error.y), 2.0 * RoundingError(5));
  EXPECT_LE(std::abs(hit_point0.error.z), 8.8e-08 * RoundingError(3));
}

TEST(Sphere, ComputeHitPointOnXYOrigin) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  Point origin(0.0, 0.0, 2.0);
  Vector direction0(0.0, 0.0, 1.0);
  Ray ray0(origin, direction0);

  Geometry::ComputeHitPointResult hit_point0 =
      sphere->ComputeHitPoint(ray0, 3.0, nullptr);
  EXPECT_EQ(ray0.Endpoint(3.0), hit_point0.point);
  EXPECT_EQ(hit_point0.error.x, 0.0);
  EXPECT_EQ(hit_point0.error.y, 0.0);
  EXPECT_EQ(hit_point0.error.z, 2.0 * RoundingError(1));

  Vector direction1(0.0, 0.0, -1.0);
  Ray ray1(origin, direction1);

  Geometry::ComputeHitPointResult hit_point1 =
      sphere->ComputeHitPoint(ray1, 1.0, nullptr);
  EXPECT_EQ(ray1.Endpoint(1.0), hit_point1.point);
  EXPECT_EQ(hit_point1.error.x, 0.0);
  EXPECT_EQ(hit_point1.error.y, 0.0);
  EXPECT_EQ(hit_point1.error.z, 2.0 * RoundingError(1));
}

TEST(Sphere, GetMaterial) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  const Material* front = sphere->GetMaterial(FRONT_FACE);
  EXPECT_EQ(front_material.Get(), front);

  const Material* back = sphere->GetMaterial(BACK_FACE);
  EXPECT_EQ(back_material.Get(), back);
}

TEST(Sphere, GetEmissiveMaterial) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  const EmissiveMaterial* front = sphere->GetEmissiveMaterial(FRONT_FACE);
  EXPECT_EQ(front_emissive_material.Get(), front);

  const EmissiveMaterial* back = sphere->GetEmissiveMaterial(BACK_FACE);
  EXPECT_EQ(back_emissive_material.Get(), back);
}

TEST(Sphere, ComputeSurfaceArea) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  std::optional<visual_t> surface_area0 =
      sphere->ComputeSurfaceArea(FRONT_FACE, nullptr);
  ASSERT_TRUE(surface_area0);
  EXPECT_NEAR(16.0 * std::numbers::pi, *surface_area0, 0.001);

  Matrix model_to_world = Matrix::Scalar(2.0, 2.0, 2.0).value();
  std::optional<visual_t> surface_area1 =
      sphere->ComputeSurfaceArea(FRONT_FACE, &model_to_world);
  ASSERT_TRUE(surface_area1);
  EXPECT_NEAR(64.0 * std::numbers::pi, *surface_area1, 0.001);
}

TEST(Sphere, SampleBySolidAngle) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  MockRandom rng0;
  EXPECT_CALL(rng0, DiscardGeometric(2));
  Sampler sampler0(rng0);

  EXPECT_TRUE(std::holds_alternative<std::monostate>(
      sphere->SampleBySolidAngle(Point(0.0, 0.0, 3.0), FRONT_FACE, sampler0)));

  MockRandom rng1;
  EXPECT_CALL(rng1, DiscardGeometric(2));
  Sampler sampler1(rng1);

  EXPECT_TRUE(std::holds_alternative<std::monostate>(
      sphere->SampleBySolidAngle(Point(0.0, 0.0, 0.0), BACK_FACE, sampler1)));

  MockRandom rng2;
  {
    InSequence s;
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(Return(0.5));
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(Return(0.5));
  }

  Sampler sampler2(rng2);
  std::variant<std::monostate, Point, Vector> sample2 =
      sphere->SampleBySolidAngle(Point(0.0, 0.0, 0.0), FRONT_FACE, sampler2);
  Vector vector2 = std::get<Vector>(sample2);
  EXPECT_NEAR(vector2.x, 0.0, 0.001);
  EXPECT_NEAR(vector2.y, 0.488296, 0.001);
  EXPECT_NEAR(vector2.z, 0.872677, 0.001);

  MockRandom rng3;
  {
    InSequence s;
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(Return(0.5));
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(Return(0.5));
  }

  Sampler sampler3(rng3);
  std::variant<std::monostate, Point, Vector> sample3 =
      sphere->SampleBySolidAngle(Point(0.0, 0.0, 3.0), BACK_FACE, sampler3);
  EXPECT_EQ(Point(2.0, 0.0, 3.0), std::get<Point>(sample3));
}

TEST(Sphere, ComputePdfBySolidAngle) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();

  std::optional<visual_t> pdf0 = sphere->ComputePdfBySolidAngle(
      Point(0.0, 0.0, 3.0), BACK_FACE, nullptr, Point(0.0, 0.0, 1.0));
  ASSERT_TRUE(pdf0);
  EXPECT_NEAR(0.0795, *pdf0, 0.01);

  std::optional<visual_t> pdf1 = sphere->ComputePdfBySolidAngle(
      Point(0.0, 0.0, 0.0), FRONT_FACE, nullptr, Point(0.0, 0.0, 1.0));
  ASSERT_TRUE(pdf1);
  EXPECT_NEAR(0.6250, *pdf1, 0.01);
}

TEST(Sphere, GetBounds) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();
  BoundingBox bounds = sphere->ComputeBounds(nullptr);
  EXPECT_EQ(Point(-2.0, -2.0, 1.0), bounds.lower);
  EXPECT_EQ(Point(2.0, 2.0, 5.0), bounds.upper);
}

TEST(Sphere, GetBoundsWithTransform) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();
  Matrix transform = Matrix::Scalar(2.0, 2.0, 1.0).value();
  BoundingBox bounds = sphere->ComputeBounds(&transform);
  EXPECT_EQ(Point(-4.0, -4.0, 1.0), bounds.lower);
  EXPECT_EQ(Point(4.0, 4.0, 5.0), bounds.upper);
}

TEST(Sphere, GetFaces) {
  ReferenceCounted<Geometry> sphere = MakeSimpleSphere();
  std::span<const face_t> faces = sphere->GetFaces();
  ASSERT_EQ(2u, faces.size());
  EXPECT_EQ(FRONT_FACE, faces[0]);
  EXPECT_EQ(BACK_FACE, faces[1]);
}

}  // namespace
}  // namespace geometry
}  // namespace iris
