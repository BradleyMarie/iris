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
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(3.0, hit->distance);
  ASSERT_EQ(nullptr, hit->next);

  EXPECT_EQ(FRONT_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, iris::testing::BackFace(*hit));
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
      iris::Point(0.0, 0.0, 0.0), FRONT_FACE, nullptr);
  EXPECT_FALSE(texture_coordinates);
}

TEST(Sphere, ComputeShadingNormal) {
  auto sphere = SimpleSphere();

  auto front_normal = sphere->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_EQ(front_normal_map.Get(),
            std::get<const iris::NormalMap*>(front_normal));

  auto back_normal = sphere->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_EQ(back_normal_map.Get(),
            std::get<const iris::NormalMap*>(back_normal));
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

TEST(Sphere, SampleFace) {
  auto sphere = SimpleSphere();

  iris::random::MockRandom rng0;
  {
    testing::InSequence s;
    EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.5));
    EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.5));
  }

  iris::Sampler sampler0(rng0);
  auto point0 = sphere->SampleFace(FRONT_FACE, sampler0);
  EXPECT_EQ(iris::Point(2.0, 0.0, 3.0), point0.value());

  iris::random::MockRandom rng1;
  {
    testing::InSequence s;
    EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.0));
    EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.4));
  }

  iris::Sampler sampler1(rng1);
  auto point1 = sphere->SampleFace(FRONT_FACE, sampler1);
  EXPECT_EQ(iris::Point(0.0, 0.0, 1.0), point1.value());

  iris::random::MockRandom rng2;
  {
    testing::InSequence s;
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(testing::Return(1.0));
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(testing::Return(0.3));
  }

  iris::Sampler sampler2(rng2);
  auto point2 = sphere->SampleFace(FRONT_FACE, sampler2);
  EXPECT_EQ(iris::Point(0.0, 0.0, 5.0), point2.value());
}

TEST(Sphere, ComputeArea) {
  auto sphere = SimpleSphere();
  EXPECT_NEAR(50.2654825, sphere->ComputeArea(FRONT_FACE).value(), 0.001);
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