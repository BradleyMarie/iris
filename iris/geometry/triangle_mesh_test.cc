#include "iris/geometry/triangle_mesh.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/materials/mock_material.h"
#include "iris/normal_maps/mock_normal_map.h"
#include "iris/random/mock_random.h"
#include "iris/testing/hit_allocator.h"

struct AdditionalData {
  std::array<iris::geometric_t, 3> barycentric_coordinates;
  iris::Vector surface_normal;
};

static const iris::face_t FRONT_FACE = 0u;
static const iris::face_t BACK_FACE = 1u;

class AlphaHits : public iris::textures::ValueTexture2D<bool> {
 public:
  bool Evaluate(const iris::TextureCoordinates& coords) const override {
    return true;
  }
};

class AlphaMisses : public iris::textures::ValueTexture2D<bool> {
 public:
  bool Evaluate(const iris::TextureCoordinates& coords) const override {
    return false;
  }
};

class Triangle : public ::testing::Test {
 protected:
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

  iris::ReferenceCounted<iris::Geometry> SimpleTriangle() {
    auto triangles = iris::geometry::AllocateTriangleMesh(
        {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
          iris::Point(0.0, 1.0, 0.0)}},
        {{{0, 1, 2}, {0, 1, 1}}}, {},
        {{{static_cast<iris::geometric>(0.0),
           static_cast<iris::geometric>(0.0)},
          {static_cast<iris::geometric>(1.0),
           static_cast<iris::geometric>(0.0)},
          {static_cast<iris::geometric>(0.0),
           static_cast<iris::geometric>(1.0)}}},
        iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
        front_material, back_material, front_emissive_material,
        back_emissive_material, front_normal_map, back_normal_map);
    EXPECT_EQ(triangles.size(), 1u);
    return triangles.front();
  }
};

TEST_F(Triangle, MissesOnEdge) {
  auto triangle = SimpleTriangle();

  iris::Point origin(-0.5, -0.5, 0.0);
  iris::Vector direction(0.5, 0.5, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = triangle->Trace(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST_F(Triangle, MissesBelow) {
  auto triangle = SimpleTriangle();

  iris::Point origin(0.5, -1.0, -1.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = triangle->Trace(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST_F(Triangle, MissesAbove) {
  auto triangle = SimpleTriangle();

  iris::Point origin(0.5, 1.0, -1.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = triangle->Trace(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST_F(Triangle, MissesLeft) {
  auto triangle = SimpleTriangle();

  iris::Point origin(-1.0, 0.5, -1.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = triangle->Trace(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST_F(Triangle, MissesRight) {
  auto triangle = SimpleTriangle();

  iris::Point origin(1.0, 0.5, -1.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);
  auto* hit = triangle->Trace(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST_F(Triangle, HitsXDominantFront) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0),
        iris::Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {}, {},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  iris::Point origin(1.0, 0.25, 0.25);
  iris::Vector direction(-1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangles.front()->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(FRONT_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, iris::testing::BackFace(*hit));

  const auto* additional_data =
      static_cast<const AdditionalData*>(iris::testing::AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(1.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST_F(Triangle, HitsXDominantBack) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0),
        iris::Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {}, {},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  iris::Point origin(-1.0, 0.25, 0.25);
  iris::Vector direction(1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangles.front()->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit));

  const auto* additional_data =
      static_cast<const AdditionalData*>(iris::testing::AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(-1.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST_F(Triangle, HitsYDominantFront) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 0.0, 1.0),
        iris::Point(1.0, 0.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  iris::Point origin(0.25, 1.0, 0.25);
  iris::Vector direction(0.0, -1.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangles.front()->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(FRONT_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, iris::testing::BackFace(*hit));

  const auto* additional_data =
      static_cast<const AdditionalData*>(iris::testing::AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(1.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST_F(Triangle, HitsYDominantBack) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 0.0, 1.0),
        iris::Point(1.0, 0.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  iris::Point origin(0.25, -1.0, 0.25);
  iris::Vector direction(0.0, 1.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangles.front()->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit));

  const auto* additional_data =
      static_cast<const AdditionalData*>(iris::testing::AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(-1.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST_F(Triangle, HitsZDominantFront) {
  auto triangle = SimpleTriangle();

  iris::Point origin(0.25, 0.25, 1.0);
  iris::Vector direction(0.0, 0.0, -1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangle->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(FRONT_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, iris::testing::BackFace(*hit));

  const auto* additional_data =
      static_cast<const AdditionalData*>(iris::testing::AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(1.0, additional_data->surface_normal.z, 0.001);
}

TEST_F(Triangle, HitsZDominantBack) {
  auto triangle = SimpleTriangle();

  iris::Point origin(0.25, 0.25, -1.0);
  iris::Vector direction(0.0, 0.0, 1.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangle->Trace(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit));

  const auto* additional_data =
      static_cast<const AdditionalData*>(iris::testing::AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(-1.0, additional_data->surface_normal.z, 0.001);
}

TEST_F(Triangle, VertexNormalsLeaves) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0),
        iris::Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {{{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}}}, {},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  iris::Point origin(-1.0, 0.25, 0.25);
  iris::Vector direction(1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangles.front()->Trace(hit_allocator);
  EXPECT_EQ(BACK_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, iris::testing::BackFace(*hit));
}

TEST_F(Triangle, VertexNormalsReverses) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0),
        iris::Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {{{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}}},
      {}, iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  iris::Point origin(-1.0, 0.25, 0.25);
  iris::Vector direction(1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  auto* hit = triangles.front()->Trace(hit_allocator);
  EXPECT_EQ(FRONT_FACE, iris::testing::FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, iris::testing::BackFace(*hit));
}

TEST_F(Triangle, AlphaHits) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0),
        iris::Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0),
         static_cast<iris::geometric>(0.0)}}},
      iris::MakeReferenceCounted<AlphaHits>(), back_material, front_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  iris::Point origin(1.0, 0.25, 0.25);
  iris::Vector direction(-1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  EXPECT_TRUE(triangles.front()->Trace(hit_allocator));
}

TEST_F(Triangle, AlphaMisses) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0),
        iris::Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0),
         static_cast<iris::geometric>(0.0)}}},
      iris::MakeReferenceCounted<AlphaMisses>(), back_material, front_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  iris::Point origin(1.0, 0.25, 0.25);
  iris::Vector direction(-1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  EXPECT_FALSE(triangles.front()->Trace(hit_allocator));
}

TEST_F(Triangle, AlphaNoUVHits) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0),
        iris::Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {}, {}, iris::MakeReferenceCounted<AlphaMisses>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  iris::Point origin(1.0, 0.25, 0.25);
  iris::Vector direction(-1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  auto hit_allocator = iris::testing::MakeHitAllocator(ray);

  EXPECT_TRUE(triangles.front()->Trace(hit_allocator));
}

TEST_F(Triangle, ComputeSurfaceNormal) {
  auto triangle = SimpleTriangle();

  AdditionalData additional_data0{{1.0, 0.0, 0.0}, iris::Vector(0.0, 0.0, 1.0)};
  auto front_normal = triangle->ComputeSurfaceNormal(
      iris::Point(0.0, 0.0, 0.0), FRONT_FACE, &additional_data0);
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), front_normal);

  AdditionalData additional_data1{{1.0, 0.0, 0.0},
                                  iris::Vector(0.0, 0.0, -1.0)};
  auto back_normal = triangle->ComputeSurfaceNormal(
      iris::Point(0.0, 0.0, 0.0), BACK_FACE, &additional_data1);
  EXPECT_EQ(iris::Vector(0.0, 0.0, -1.0), back_normal);
}

TEST_F(Triangle, ComputeTextureCoordinatesNone) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
        iris::Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  AdditionalData additional_data{{1.0, 0.0, 0.0}, iris::Vector(0.0, 0.0, 1.0)};
  auto texture_coordinates = triangles.front()->ComputeTextureCoordinates(
      iris::Point(0.0, 0.0, 0.0), std::nullopt, FRONT_FACE, &additional_data);
  EXPECT_FALSE(texture_coordinates);
}

TEST_F(Triangle, ComputeTextureCoordinates) {
  auto triangle = SimpleTriangle();

  AdditionalData additional_data0{{1.0, 0.0, 0.0}, iris::Vector(0.0, 0.0, 1.0)};
  auto texture_coordinates0 = triangle->ComputeTextureCoordinates(
      iris::Point(0.0, 0.0, 0.0), std::nullopt, FRONT_FACE, &additional_data0);
  EXPECT_EQ(0.0, texture_coordinates0->uv[0]);
  EXPECT_EQ(0.0, texture_coordinates0->uv[1]);
  EXPECT_FALSE(texture_coordinates0->differentials);

  AdditionalData additional_data1{{0.0, 1.0, 0.0}, iris::Vector(0.0, 0.0, 1.0)};
  auto texture_coordinates1 = triangle->ComputeTextureCoordinates(
      iris::Point(0.0, 0.0, 0.0), std::nullopt, FRONT_FACE, &additional_data1);
  EXPECT_EQ(1.0, texture_coordinates1->uv[0]);
  EXPECT_EQ(0.0, texture_coordinates1->uv[1]);
  EXPECT_FALSE(texture_coordinates1->differentials);

  AdditionalData additional_data2{{0.0, 0.0, 1.0}, iris::Vector(0.0, 0.0, 1.0)};
  auto texture_coordinates2 = triangle->ComputeTextureCoordinates(
      iris::Point(0.0, 0.0, 0.0), std::nullopt, FRONT_FACE, &additional_data2);
  EXPECT_EQ(0.0, texture_coordinates2->uv[0]);
  EXPECT_EQ(1.0, texture_coordinates2->uv[1]);
  EXPECT_FALSE(texture_coordinates2->differentials);

  AdditionalData additional_data3{{1.0, 0.0, 0.0}, iris::Vector(0.0, 0.0, 1.0)};
  auto texture_coordinates3 = triangle->ComputeTextureCoordinates(
      iris::Point(0.0, 0.0, 0.0),
      {{iris::Point(1.0, 0.0, 0.0), iris::Point(0.0, 1.0, 0.0)}}, FRONT_FACE,
      &additional_data3);
  EXPECT_EQ(0.0, texture_coordinates3->uv[0]);
  EXPECT_EQ(0.0, texture_coordinates3->uv[1]);
  ASSERT_TRUE(texture_coordinates3->differentials);
  EXPECT_EQ(1.0, texture_coordinates3->differentials->du_dx);
  EXPECT_EQ(0.0, texture_coordinates3->differentials->du_dy);
  EXPECT_EQ(0.0, texture_coordinates3->differentials->dv_dx);
  EXPECT_EQ(1.0, texture_coordinates3->differentials->dv_dy);
}

TEST_F(Triangle, ComputeShadingNormalNoUVs) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
        iris::Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());

  auto front_normal =
      triangles.front()->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  EXPECT_FALSE(front_normal.dp_duv);
  EXPECT_EQ(nullptr, front_normal.normal_map);

  auto back_normal =
      triangles.front()->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.surface_normal);
  EXPECT_FALSE(back_normal.dp_duv);
  EXPECT_EQ(nullptr, back_normal.normal_map);
}

TEST_F(Triangle, ComputeShadingNormalUVsDegenerate) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
        iris::Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0),
         static_cast<iris::geometric>(0.0)}}},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());

  auto front_normal =
      triangles.front()->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  EXPECT_FALSE(front_normal.dp_duv);
  EXPECT_EQ(nullptr, front_normal.normal_map);

  auto back_normal =
      triangles.front()->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.surface_normal);
  EXPECT_FALSE(back_normal.dp_duv);
  EXPECT_EQ(nullptr, back_normal.normal_map);
}

TEST_F(Triangle, ComputeShadingNormalNone) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
        iris::Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(1.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0),
         static_cast<iris::geometric>(1.0)}}},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());

  auto front_normal =
      triangles.front()->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  ASSERT_TRUE(front_normal.dp_duv);
  EXPECT_EQ(1.0, front_normal.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal.normal_map);

  auto back_normal =
      triangles.front()->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.surface_normal);
  ASSERT_TRUE(back_normal.dp_duv);
  EXPECT_EQ(1.0, back_normal.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal.normal_map);
}

TEST_F(Triangle, ComputeShadingNormalZeroLength) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
        iris::Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}},
      {{{static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(1.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0),
         static_cast<iris::geometric>(1.0)}}},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());

  AdditionalData additional_data{{1.0, 1.0, 1.0}, iris::Vector(1.0, 0.0, 0.0)};

  auto front_normal =
      triangles.front()->ComputeShadingNormal(FRONT_FACE, &additional_data);
  EXPECT_FALSE(front_normal.surface_normal);
  ASSERT_TRUE(front_normal.dp_duv);
  EXPECT_EQ(1.0, front_normal.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal.normal_map);

  auto back_normal =
      triangles.front()->ComputeShadingNormal(BACK_FACE, &additional_data);
  EXPECT_FALSE(back_normal.surface_normal);
  ASSERT_TRUE(back_normal.dp_duv);
  EXPECT_EQ(1.0, back_normal.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal.normal_map);
}

TEST_F(Triangle, ComputeShadingNormalFromMap) {
  auto triangle = SimpleTriangle();

  auto front_normal = triangle->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  ASSERT_TRUE(front_normal.dp_duv);
  EXPECT_EQ(1.0, front_normal.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.z);
  EXPECT_EQ(front_normal_map.Get(), front_normal.normal_map);

  auto back_normal = triangle->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.surface_normal);
  ASSERT_TRUE(back_normal.dp_duv);
  EXPECT_EQ(1.0, back_normal.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal.dp_duv->second.z);
  EXPECT_EQ(back_normal_map.Get(), back_normal.normal_map);
}

TEST_F(Triangle, ComputeShadingNormalFromNormals) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
        iris::Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}},
      {{{static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(1.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0),
         static_cast<iris::geometric>(1.0)}}},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());

  AdditionalData additional_data0_front{{1.0, 0.0, 0.0},
                                        iris::Vector(1.0, 0.0, 0.0)};
  auto front_normal0 = triangles.front()->ComputeShadingNormal(
      FRONT_FACE, &additional_data0_front);
  EXPECT_EQ(iris::Vector(1.0, 0.0, 0.0), front_normal0.surface_normal);
  ASSERT_TRUE(front_normal0.dp_duv);
  EXPECT_EQ(1.0, front_normal0.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal0.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal0.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal0.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal0.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal0.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal0.normal_map);

  AdditionalData additional_data0_back{{1.0, 0.0, 0.0},
                                       iris::Vector(-1.0, 0.0, 0.0)};
  auto back_normal0 = triangles.front()->ComputeShadingNormal(
      BACK_FACE, &additional_data0_back);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0), back_normal0.surface_normal);
  ASSERT_TRUE(back_normal0.dp_duv);
  EXPECT_EQ(1.0, back_normal0.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal0.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal0.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal0.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal0.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal0.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal0.normal_map);

  AdditionalData additional_data1_front{{0.0, 1.0, 0.0},
                                        iris::Vector(0.0, 1.0, 0.0)};
  auto front_normal1 = triangles.front()->ComputeShadingNormal(
      FRONT_FACE, &additional_data1_front);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), front_normal1.surface_normal);
  ASSERT_TRUE(front_normal1.dp_duv);
  EXPECT_EQ(1.0, front_normal1.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal1.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal1.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal1.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal1.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal1.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal1.normal_map);

  AdditionalData additional_data1_back{{0.0, 1.0, 0.0},
                                       iris::Vector(0.0, -1.0, 0.0)};
  auto back_normal1 = triangles.front()->ComputeShadingNormal(
      BACK_FACE, &additional_data1_back);
  EXPECT_EQ(iris::Vector(0.0, -1.0, 0.0), back_normal1.surface_normal);
  ASSERT_TRUE(back_normal1.dp_duv);
  EXPECT_EQ(1.0, back_normal1.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal1.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal1.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal1.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal1.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal1.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal1.normal_map);

  AdditionalData additional_data2_front{{0.0, 0.0, 1.0},
                                        iris::Vector(0.0, 0.0, 1.0)};
  auto front_normal2 = triangles.front()->ComputeShadingNormal(
      FRONT_FACE, &additional_data2_front);
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), front_normal2.surface_normal);
  ASSERT_TRUE(front_normal2.dp_duv);
  EXPECT_EQ(1.0, front_normal2.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal2.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal2.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal2.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal2.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal2.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal2.normal_map);

  AdditionalData additional_data2_back{{0.0, 0.0, 1.0},
                                       iris::Vector(0.0, 0.0, -1.0)};
  auto back_normal2 = triangles.front()->ComputeShadingNormal(
      BACK_FACE, &additional_data2_back);
  EXPECT_EQ(iris::Vector(0.0, 0.0, -1.0), back_normal2.surface_normal);
  ASSERT_TRUE(back_normal2.dp_duv);
  EXPECT_EQ(1.0, back_normal2.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal2.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal2.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal2.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal2.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal2.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal2.normal_map);
}

TEST_F(Triangle, ComputeHitPoint) {
  auto triangle = SimpleTriangle();

  iris::Point origin(1.0, 0.25, 0.25);
  iris::Vector direction(-1.0, 0.0, 0.0);
  iris::Ray ray(origin, direction);

  AdditionalData additional_data0{{0.0, 0.0, 1.0},
                                  iris::Vector(0.0, 0.0, -1.0)};
  auto hit_point0 = triangle->ComputeHitPoint(ray, 1.0, &additional_data0);
  EXPECT_EQ(hit_point0.point, iris::Point(0.0, 1.0, 0.0));
  EXPECT_EQ(hit_point0.error.x, 0.0);
  EXPECT_LE(std::abs(hit_point0.error.y), 1.0 * iris::RoundingError(7));
  EXPECT_EQ(hit_point0.error.z, 0.0);
}

TEST_F(Triangle, GetMaterial) {
  auto triangle = SimpleTriangle();

  auto front = triangle->GetMaterial(FRONT_FACE, nullptr);
  EXPECT_EQ(front_material.Get(), front);

  auto back = triangle->GetMaterial(BACK_FACE, nullptr);
  EXPECT_EQ(back_material.Get(), back);
}

TEST_F(Triangle, IsEmissive) {
  auto triangles = iris::geometry::AllocateTriangleMesh(
      {{iris::Point(0.0, 0.0, 0.0), iris::Point(1.0, 0.0, 0.0),
        iris::Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}},
      {{{static_cast<iris::geometric>(0.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(1.0), static_cast<iris::geometric>(0.0)},
        {static_cast<iris::geometric>(0.0),
         static_cast<iris::geometric>(1.0)}}},
      iris::ReferenceCounted<iris::textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      iris::ReferenceCounted<iris::EmissiveMaterial>(),
      iris::ReferenceCounted<iris::NormalMap>(),
      iris::ReferenceCounted<iris::NormalMap>());
  EXPECT_TRUE(triangles.front()->IsEmissive(FRONT_FACE));
  EXPECT_FALSE(triangles.front()->IsEmissive(BACK_FACE));
}

TEST_F(Triangle, GetEmissiveMaterial) {
  auto triangle = SimpleTriangle();

  auto front = triangle->GetEmissiveMaterial(FRONT_FACE, nullptr);
  EXPECT_EQ(front_emissive_material.Get(), front);

  auto back = triangle->GetEmissiveMaterial(BACK_FACE, nullptr);
  EXPECT_EQ(back_emissive_material.Get(), back);
}

TEST_F(Triangle, SampleBySolidAngle) {
  auto triangle = SimpleTriangle();

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillRepeatedly(testing::Return(0.0));

  iris::Sampler sampler0(rng0);
  auto sample0 = triangle->SampleBySolidAngle(iris::Point(-1.0, -1.0, -1.0),
                                              FRONT_FACE, sampler0);
  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), std::get<iris::Point>(sample0));

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillRepeatedly(testing::Return(1.0));

  iris::Sampler sampler1(rng1);
  auto sample1 = triangle->SampleBySolidAngle(iris::Point(-1.0, -1.0, -1.0),
                                              FRONT_FACE, sampler1);
  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), std::get<iris::Point>(sample1));

  iris::random::MockRandom rng2;
  {
    testing::InSequence s;
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(testing::Return(1.0));
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(testing::Return(0.0));
  }

  iris::Sampler sampler2(rng2);
  auto sample2 = triangle->SampleBySolidAngle(iris::Point(-1.0, -1.0, -1.0),
                                              FRONT_FACE, sampler2);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), std::get<iris::Point>(sample2));

  iris::random::MockRandom rng3;
  {
    testing::InSequence s;
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(testing::Return(0.0));
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(testing::Return(1.0));
  }

  iris::Sampler sampler3(rng3);
  auto sample3 = triangle->SampleBySolidAngle(iris::Point(-1.0, -1.0, -1.0),
                                              FRONT_FACE, sampler3);
  EXPECT_EQ(iris::Point(0.0, 1.0, 0.0), std::get<iris::Point>(sample3));

  iris::random::MockRandom rng4;
  EXPECT_CALL(rng4, NextGeometric()).WillRepeatedly(testing::Return(0.25));

  iris::Sampler sampler4(rng4);
  auto sample4 = triangle->SampleBySolidAngle(iris::Point(-1.0, -1.0, -1.0),
                                              FRONT_FACE, sampler4);
  EXPECT_EQ(iris::Point(0.25, 0.25, 0.0), std::get<iris::Point>(sample4));

  iris::random::MockRandom rng5;
  EXPECT_CALL(rng5, NextGeometric()).WillRepeatedly(testing::Return(0.75));

  iris::Sampler sampler5(rng5);
  auto sample5 = triangle->SampleBySolidAngle(iris::Point(-1.0, -1.0, -1.0),
                                              FRONT_FACE, sampler5);
  EXPECT_EQ(iris::Point(0.25, 0.25, 0.0), std::get<iris::Point>(sample4));
}

TEST_F(Triangle, ComputePdfBySolidAngle) {
  auto triangle = SimpleTriangle();
  AdditionalData additional_data{{0.0, 0.0, 1.0}, iris::Vector(0.0, 0.0, 1.0)};

  auto pdf0 = triangle->ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 1.0),
                                               FRONT_FACE, &additional_data,
                                               iris::Point(0.0, 0.0, 0.0));
  EXPECT_NEAR(2.0, pdf0.value(), 0.001);

  auto pdf1 = triangle->ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 1.0),
                                               FRONT_FACE, &additional_data,
                                               iris::Point(1.0, 0.0, 0.0));
  EXPECT_NEAR(5.6568541, pdf1.value(), 0.001);

  auto pdf2 = triangle->ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 1.0),
                                               FRONT_FACE, &additional_data,
                                               iris::Point(0.25, 0.25, 0.0));
  EXPECT_NEAR(2.3864853, pdf2.value(), 0.001);
}

TEST_F(Triangle, GetBoundsWithTransform) {
  auto triangle = SimpleTriangle();
  auto transform = iris::Matrix::Scalar(2.0, 2.0, 2.0).value();
  auto bounds = triangle->ComputeBounds(transform);
  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), bounds.lower);
  EXPECT_EQ(iris::Point(2.0, 2.0, 0.0), bounds.upper);
}

TEST_F(Triangle, GetFaces) {
  auto triangle = SimpleTriangle();
  auto faces = triangle->GetFaces();
  ASSERT_EQ(2u, faces.size());
  EXPECT_EQ(FRONT_FACE, faces[0]);
  EXPECT_EQ(BACK_FACE, faces[1]);
}