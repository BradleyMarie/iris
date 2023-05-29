#include "iris/geometry/sphere.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/materials/mock_material.h"
#include "iris/normal_maps/mock_normal_map.h"
#include "iris/random/mock_random.h"
#include "iris/testing/hit_allocator.h"

typedef std::array<iris::geometric_t, 3> AdditionalData;
static const iris::face_t FRONT_FACE = 0u;
static const iris::face_t BACK_FACE = 1u;

iris::ReferenceCounted<iris::Material> front_material =
    iris::MakeReferenceCounted<iris::materials::MockMaterial>();
iris::ReferenceCounted<iris::Material> back_material =
    iris::MakeReferenceCounted<iris::materials::MockMaterial>();
iris::ReferenceCounted<iris::EmissiveMaterial> front_emissive_material =
    iris::MakeReferenceCounted<
        iris::emissive_materials::MockEmissiveMaterial>();
iris::ReferenceCounted<iris::EmissiveMaterial> back_emissive_material =
    iris::MakeReferenceCounted<
        iris::emissive_materials::MockEmissiveMaterial>();
iris::ReferenceCounted<iris::NormalMap> front_normal_map =
    iris::MakeReferenceCounted<iris::normal_maps::MockNormalMap>();
iris::ReferenceCounted<iris::NormalMap> back_normal_map =
    iris::MakeReferenceCounted<iris::normal_maps::MockNormalMap>();

iris::ReferenceCounted<iris::Geometry> SimpleSphere() {
  return iris::geometry::AllocateSphere(
      iris::Point(0.0, 0.0, 3.0), 2.0, front_material, back_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);
}

TEST(Sphere, MissesCompletely) {
  auto sphere = SimpleSphere();

  iris::Point origin(0.0, 100.0, 100.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = sphere->Trace(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Sphere, MissesOnEdge) {
  auto sphere = SimpleSphere();

  iris::Point origin(0.0, 2.0, 0.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = sphere->Trace(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Sphere, Inside) {
  auto sphere = SimpleSphere();

  iris::Point origin(0.0, 0.0, 2.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = sphere->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(-1.0, hit->distance);

  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit));

  ASSERT_NE(nullptr, hit->next);
  EXPECT_EQ(3.0, hit->next->distance);

  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit->next));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit->next));
}

TEST(Sphere, InFront) {
  auto sphere = SimpleSphere();

  iris::Point origin(0.0, 0.0, 0.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = sphere->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);

  EXPECT_EQ(FRONT_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, iris::testing::BackFace(*hit));

  ASSERT_NE(nullptr, hit->next);
  EXPECT_EQ(5.0, hit->next->distance);

  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit->next));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit->next));
}

TEST(Sphere, Behind) {
  auto sphere = SimpleSphere();

  iris::Point origin(0.0, 0.0, 6.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = sphere->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(-5.0, hit->distance);

  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit));

  ASSERT_NE(nullptr, hit->next);
  EXPECT_EQ(-1.0, hit->next->distance);

  EXPECT_EQ(FRONT_FACE, iris::testing::FrontFace(*hit->next));
  EXPECT_EQ(BACK_FACE, iris::testing::BackFace(*hit->next));
}

TEST(Sphere, ComputeSurfaceNormal) {
  auto sphere = SimpleSphere();

  auto front_normal = sphere->ComputeSurfaceNormal(iris::Point(0.0, 0.0, 0.0),
                                                   FRONT_FACE, nullptr);
  EXPECT_EQ(iris::Vector(0.0, 0.0, -1.0), front_normal);

  auto back_normal = sphere->ComputeSurfaceNormal(iris::Point(0.0, 0.0, 0.0),
                                                  BACK_FACE, nullptr);
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), back_normal);
}

TEST(Sphere, ComputeTextureCoordinatesNone) {
  auto sphere = SimpleSphere();
  auto texture_coordinates = sphere->ComputeTextureCoordinates(
      iris::Point(0.0, 0.0, 0.0), std::nullopt, FRONT_FACE, nullptr);
  EXPECT_FALSE(texture_coordinates);
}

TEST(Sphere, ComputeShadingNormal) {
  auto sphere = SimpleSphere();

  auto front_normal = sphere->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.geometry);
  EXPECT_EQ(front_normal_map.Get(), front_normal.normal_map);

  auto back_normal = sphere->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.geometry);
  EXPECT_EQ(back_normal_map.Get(), back_normal.normal_map);
}

TEST(Sphere, GetMaterial) {
  auto sphere = SimpleSphere();

  auto front = sphere->GetMaterial(FRONT_FACE, nullptr);
  EXPECT_EQ(front_material.Get(), front);

  auto back = sphere->GetMaterial(BACK_FACE, nullptr);
  EXPECT_EQ(back_material.Get(), back);
}

TEST(Sphere, IsEmissive) {
  auto sphere = iris::geometry::AllocateSphere(
      iris::Point(0.0, 0.0, 0.0), 1.0, back_material, front_material,
      front_emissive_material, iris::ReferenceCounted<iris::EmissiveMaterial>(),
      iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());
  EXPECT_TRUE(sphere->IsEmissive(FRONT_FACE));
  EXPECT_FALSE(sphere->IsEmissive(BACK_FACE));
}

TEST(Sphere, GetEmissiveMaterial) {
  auto sphere = SimpleSphere();

  auto front = sphere->GetEmissiveMaterial(FRONT_FACE, nullptr);
  EXPECT_EQ(front_emissive_material.Get(), front);

  auto back = sphere->GetEmissiveMaterial(BACK_FACE, nullptr);
  EXPECT_EQ(back_emissive_material.Get(), back);
}

TEST(Sphere, SampleBySolidAngle) {
  auto sphere = SimpleSphere();

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, DiscardGeometric(2));
  iris::Sampler sampler0(rng0);

  EXPECT_TRUE(std::holds_alternative<std::monostate>(sphere->SampleBySolidAngle(
      iris::Point(0.0, 0.0, 3.0), FRONT_FACE, sampler0)));

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, DiscardGeometric(2));
  iris::Sampler sampler1(rng1);

  EXPECT_TRUE(std::holds_alternative<std::monostate>(sphere->SampleBySolidAngle(
      iris::Point(0.0, 0.0, 0.0), BACK_FACE, sampler1)));

  iris::random::MockRandom rng2;
  {
    testing::InSequence s;
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(testing::Return(0.5));
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(testing::Return(0.5));
  }

  iris::Sampler sampler2(rng2);
  auto sample2 = sphere->SampleBySolidAngle(iris::Point(0.0, 0.0, 0.0),
                                            FRONT_FACE, sampler2);
  auto vector2 = std::get<iris::Vector>(sample2);
  EXPECT_NEAR(vector2.x, 0.0, 0.001);
  EXPECT_NEAR(vector2.y, 0.488296, 0.001);
  EXPECT_NEAR(vector2.z, 0.872677, 0.001);

  iris::random::MockRandom rng3;
  {
    testing::InSequence s;
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(testing::Return(0.5));
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(testing::Return(0.5));
  }

  iris::Sampler sampler3(rng3);
  auto sample3 = sphere->SampleBySolidAngle(iris::Point(0.0, 0.0, 3.0),
                                            BACK_FACE, sampler3);
  EXPECT_EQ(iris::Point(2.0, 0.0, 3.0), std::get<iris::Point>(sample3));
}

TEST(Sphere, ComputePdfBySolidAngle) {
  auto sphere = SimpleSphere();

  auto pdf0 = sphere->ComputePdfBySolidAngle(
      iris::Point(0.0, 0.0, 3.0), BACK_FACE, iris::Point(0.0, 0.0, 1.0));
  ASSERT_TRUE(pdf0);
  EXPECT_NEAR(0.0795, *pdf0, 0.01);

  auto pdf1 = sphere->ComputePdfBySolidAngle(
      iris::Point(0.0, 0.0, 0.0), FRONT_FACE, iris::Point(0.0, 0.0, 1.0));
  ASSERT_TRUE(pdf1);
  EXPECT_NEAR(0.6250, *pdf1, 0.01);
}

TEST(Sphere, GetBoundsWithTransform) {
  auto sphere = SimpleSphere();
  auto transform = iris::Matrix::Scalar(2.0, 2.0, 1.0).value();
  auto bounds = sphere->ComputeBounds(transform);
  EXPECT_EQ(iris::Point(-4.0, -4.0, 1.0), bounds.lower);
  EXPECT_EQ(iris::Point(4.0, 4.0, 5.0), bounds.upper);
}

TEST(Sphere, GetFaces) {
  auto sphere = SimpleSphere();
  auto faces = sphere->GetFaces();
  ASSERT_EQ(2u, faces.size());
  EXPECT_EQ(FRONT_FACE, faces[0]);
  EXPECT_EQ(BACK_FACE, faces[1]);
}