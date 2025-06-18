#include "iris/geometry/triangle_mesh.h"

#include <array>
#include <optional>
#include <span>
#include <variant>
#include <vector>

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
using ::iris::testing::AdditionalData;
using ::iris::testing::BackFace;
using ::iris::testing::FrontFace;
using ::iris::testing::MakeHitAllocator;
using ::testing::InSequence;
using ::testing::IsEmpty;
using ::testing::Return;

struct AdditionalDataContents {
  std::array<geometric_t, 3> barycentric_coordinates;
  Vector surface_normal;
};

static const face_t FRONT_FACE = 0u;
static const face_t BACK_FACE = 1u;

class AlphaHits : public textures::ValueTexture2D<bool> {
 public:
  bool Evaluate(const TextureCoordinates& coords) const override {
    return true;
  }
};

class AlphaMisses : public textures::ValueTexture2D<bool> {
 public:
  bool Evaluate(const TextureCoordinates& coords) const override {
    return false;
  }
};

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

ReferenceCounted<Geometry> MakeSimpleTriangle() {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}, {0, 1, 1}}}, {},
      {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(1.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(1.0)}}},
      ReferenceCounted<textures::ValueTexture2D<bool>>(), front_material,
      back_material, front_emissive_material, back_emissive_material,
      front_normal_map, back_normal_map);
  EXPECT_EQ(triangles.size(), 1u);
  return triangles.front();
}

TEST(Triangle, Empty) {
  EXPECT_THAT(
      AllocateTriangleMesh(
          {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
          {{{0, 1, 1}}}, {},
          {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
            {static_cast<geometric>(1.0), static_cast<geometric>(0.0)},
            {static_cast<geometric>(0.0), static_cast<geometric>(1.0)}}},
          ReferenceCounted<textures::ValueTexture2D<bool>>(), front_material,
          back_material, front_emissive_material, back_emissive_material,
          front_normal_map, back_normal_map),
      IsEmpty());
}

TEST(Triangle, MissesOnEdge) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(-0.5, -0.5, 0.0);
  Vector direction(0.5, 0.5, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = triangle->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Triangle, MissesBelow) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(0.5, -1.0, -1.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = triangle->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Triangle, MissesAbove) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(0.5, 1.0, -1.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = triangle->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Triangle, MissesLeft) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(-1.0, 0.5, -1.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = triangle->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Triangle, MissesRight) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(1.0, 0.5, -1.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);
  Hit* hit = triangle->TraceAllHits(hit_allocator);
  EXPECT_EQ(nullptr, hit);
}

TEST(Triangle, HitsXDominantFront) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 0.0), Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {}, {}, ReferenceCounted<textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  Point origin(1.0, 0.25, 0.25);
  Vector direction(-1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangles.front()->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(FRONT_FACE, FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, BackFace(*hit));

  const AdditionalDataContents* additional_data =
      static_cast<const AdditionalDataContents*>(AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(1.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST(Triangle, HitsXDominantBack) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 0.0), Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {}, {}, ReferenceCounted<textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  Point origin(-1.0, 0.25, 0.25);
  Vector direction(1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangles.front()->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(BACK_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));

  const AdditionalDataContents* additional_data =
      static_cast<const AdditionalDataContents*>(AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(-1.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST(Triangle, HitsYDominantFront) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 1.0), Point(1.0, 0.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {}, ReferenceCounted<textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  Point origin(0.25, 1.0, 0.25);
  Vector direction(0.0, -1.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangles.front()->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(FRONT_FACE, FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, BackFace(*hit));

  const AdditionalDataContents* additional_data =
      static_cast<const AdditionalDataContents*>(AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(1.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST(Triangle, HitsYDominantBack) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 1.0), Point(1.0, 0.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {}, ReferenceCounted<textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  Point origin(0.25, -1.0, 0.25);
  Vector direction(0.0, 1.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangles.front()->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(BACK_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));

  const AdditionalDataContents* additional_data =
      static_cast<const AdditionalDataContents*>(AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(-1.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.z, 0.001);
}

TEST(Triangle, HitsZDominantFront) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(0.25, 0.25, 1.0);
  Vector direction(0.0, 0.0, -1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangle->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(FRONT_FACE, FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, BackFace(*hit));

  const AdditionalDataContents* additional_data =
      static_cast<const AdditionalDataContents*>(AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(1.0, additional_data->surface_normal.z, 0.001);
}

TEST(Triangle, HitsZDominantBack) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(0.25, 0.25, -1.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangle->TraceAllHits(hit_allocator);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);

  EXPECT_EQ(BACK_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));

  const AdditionalDataContents* additional_data =
      static_cast<const AdditionalDataContents*>(AdditionalData(*hit));
  ASSERT_NE(nullptr, additional_data);
  EXPECT_EQ(0.50, additional_data->barycentric_coordinates.at(0));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(1));
  EXPECT_EQ(0.25, additional_data->barycentric_coordinates.at(2));
  EXPECT_NEAR(0.0, additional_data->surface_normal.x, 0.001);
  EXPECT_NEAR(0.0, additional_data->surface_normal.y, 0.001);
  EXPECT_NEAR(-1.0, additional_data->surface_normal.z, 0.001);
}

TEST(Triangle, VertexNormalsLeaves) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 0.0), Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {{{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}}}, {},
      ReferenceCounted<textures::ValueTexture2D<bool>>(), back_material,
      front_material, front_emissive_material, back_emissive_material,
      front_normal_map, back_normal_map);

  Point origin(-1.0, 0.25, 0.25);
  Vector direction(1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangles.front()->TraceAllHits(hit_allocator);
  EXPECT_EQ(BACK_FACE, FrontFace(*hit));
  EXPECT_EQ(FRONT_FACE, BackFace(*hit));
}

TEST(Triangle, VertexNormalsReverses) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 0.0), Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {{{-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}}},
      {}, ReferenceCounted<textures::ValueTexture2D<bool>>(), back_material,
      front_material, front_emissive_material, back_emissive_material,
      front_normal_map, back_normal_map);

  Point origin(-1.0, 0.25, 0.25);
  Vector direction(1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  Hit* hit = triangles.front()->TraceAllHits(hit_allocator);
  EXPECT_EQ(FRONT_FACE, FrontFace(*hit));
  EXPECT_EQ(BACK_FACE, BackFace(*hit));
}

TEST(Triangle, AlphaHits) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 0.0), Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(0.0)}}},
      MakeReferenceCounted<AlphaHits>(), back_material, front_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  Point origin(1.0, 0.25, 0.25);
  Vector direction(-1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  EXPECT_TRUE(triangles.front()->TraceAllHits(hit_allocator));
}

TEST(Triangle, AlphaMisses) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 0.0), Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(0.0)}}},
      MakeReferenceCounted<AlphaMisses>(), back_material, front_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  Point origin(1.0, 0.25, 0.25);
  Vector direction(-1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  EXPECT_FALSE(triangles.front()->TraceAllHits(hit_allocator));
}

TEST(Triangle, AlphaNoUVHits) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 0.0), Point(0.0, 0.0, 1.0)}},
      {{{0, 1, 2}}}, {}, {}, MakeReferenceCounted<AlphaMisses>(), back_material,
      front_material, front_emissive_material, back_emissive_material,
      front_normal_map, back_normal_map);

  Point origin(1.0, 0.25, 0.25);
  Vector direction(-1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  HitAllocator hit_allocator = MakeHitAllocator(ray);

  EXPECT_TRUE(triangles.front()->TraceAllHits(hit_allocator));
}

TEST(Triangle, ComputeSurfaceNormal) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  AdditionalDataContents additional_data0{{1.0, 0.0, 0.0},
                                          Vector(0.0, 0.0, 1.0)};
  Vector front_normal = triangle->ComputeSurfaceNormal(
      Point(0.0, 0.0, 0.0), FRONT_FACE, &additional_data0);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), front_normal);

  AdditionalDataContents additional_data1{{1.0, 0.0, 0.0},
                                          Vector(0.0, 0.0, -1.0)};
  Vector back_normal = triangle->ComputeSurfaceNormal(
      Point(0.0, 0.0, 0.0), BACK_FACE, &additional_data1);
  EXPECT_EQ(Vector(0.0, 0.0, -1.0), back_normal);
}

TEST(Triangle, ComputeTextureCoordinatesNone) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {}, ReferenceCounted<textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  AdditionalDataContents additional_data{{1.0, 0.0, 0.0},
                                         Vector(0.0, 0.0, 1.0)};
  std::optional<TextureCoordinates> texture_coordinates =
      triangles.front()->ComputeTextureCoordinates(
          Point(0.0, 0.0, 0.0), std::nullopt, FRONT_FACE, &additional_data);
  EXPECT_FALSE(texture_coordinates);
}

TEST(Triangle, ComputeTextureCoordinates) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  AdditionalDataContents additional_data0{{1.0, 0.0, 0.0},
                                          Vector(0.0, 0.0, 1.0)};
  std::optional<TextureCoordinates> texture_coordinates0 =
      triangle->ComputeTextureCoordinates(Point(0.0, 0.0, 0.0), std::nullopt,
                                          FRONT_FACE, &additional_data0);
  EXPECT_EQ(0.0, texture_coordinates0->uv[0]);
  EXPECT_EQ(0.0, texture_coordinates0->uv[1]);
  EXPECT_FALSE(texture_coordinates0->differentials);

  AdditionalDataContents additional_data1{{0.0, 1.0, 0.0},
                                          Vector(0.0, 0.0, 1.0)};
  std::optional<TextureCoordinates> texture_coordinates1 =
      triangle->ComputeTextureCoordinates(Point(0.0, 0.0, 0.0), std::nullopt,
                                          FRONT_FACE, &additional_data1);
  EXPECT_EQ(1.0, texture_coordinates1->uv[0]);
  EXPECT_EQ(0.0, texture_coordinates1->uv[1]);
  EXPECT_FALSE(texture_coordinates1->differentials);

  AdditionalDataContents additional_data2{{0.0, 0.0, 1.0},
                                          Vector(0.0, 0.0, 1.0)};
  std::optional<TextureCoordinates> texture_coordinates2 =
      triangle->ComputeTextureCoordinates(Point(0.0, 0.0, 0.0), std::nullopt,
                                          FRONT_FACE, &additional_data2);
  EXPECT_EQ(0.0, texture_coordinates2->uv[0]);
  EXPECT_EQ(1.0, texture_coordinates2->uv[1]);
  EXPECT_FALSE(texture_coordinates2->differentials);

  AdditionalDataContents additional_data3{{1.0, 0.0, 0.0},
                                          Vector(0.0, 0.0, 1.0)};
  std::optional<TextureCoordinates> texture_coordinates3 =
      triangle->ComputeTextureCoordinates(
          Point(0.0, 0.0, 0.0), {{Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
          FRONT_FACE, &additional_data3);
  EXPECT_EQ(0.0, texture_coordinates3->uv[0]);
  EXPECT_EQ(0.0, texture_coordinates3->uv[1]);
  ASSERT_TRUE(texture_coordinates3->differentials);
  EXPECT_EQ(1.0, texture_coordinates3->differentials->du_dx);
  EXPECT_EQ(0.0, texture_coordinates3->differentials->du_dy);
  EXPECT_EQ(0.0, texture_coordinates3->differentials->dv_dx);
  EXPECT_EQ(1.0, texture_coordinates3->differentials->dv_dy);
}

TEST(Triangle, ComputeShadingNormalNoUVs) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {}, {}, ReferenceCounted<textures::ValueTexture2D<bool>>(),
      back_material, front_material, front_emissive_material,
      back_emissive_material, ReferenceCounted<NormalMap>(),
      ReferenceCounted<NormalMap>());

  Geometry::ComputeShadingNormalResult front_normal =
      triangles.front()->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  EXPECT_FALSE(front_normal.dp_duv);
  EXPECT_EQ(nullptr, front_normal.normal_map);

  Geometry::ComputeShadingNormalResult back_normal =
      triangles.front()->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.surface_normal);
  EXPECT_FALSE(back_normal.dp_duv);
  EXPECT_EQ(nullptr, back_normal.normal_map);
}

TEST(Triangle, ComputeShadingNormalUVsDegenerate) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(0.0)}}},
      ReferenceCounted<textures::ValueTexture2D<bool>>(), back_material,
      front_material, front_emissive_material, back_emissive_material,
      ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>());

  Geometry::ComputeShadingNormalResult front_normal =
      triangles.front()->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  EXPECT_FALSE(front_normal.dp_duv);
  EXPECT_EQ(nullptr, front_normal.normal_map);

  Geometry::ComputeShadingNormalResult back_normal =
      triangles.front()->ComputeShadingNormal(BACK_FACE, nullptr);
  EXPECT_FALSE(back_normal.surface_normal);
  EXPECT_FALSE(back_normal.dp_duv);
  EXPECT_EQ(nullptr, back_normal.normal_map);
}

TEST(Triangle, ComputeShadingNormalNone) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {},
      {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(1.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(1.0)}}},
      ReferenceCounted<textures::ValueTexture2D<bool>>(), back_material,
      front_material, front_emissive_material, back_emissive_material,
      ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>());

  Geometry::ComputeShadingNormalResult front_normal =
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

  Geometry::ComputeShadingNormalResult back_normal =
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

TEST(Triangle, ComputeShadingNormalZeroLength) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}},
      {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(1.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(1.0)}}},
      ReferenceCounted<textures::ValueTexture2D<bool>>(), back_material,
      front_material, front_emissive_material, back_emissive_material,
      ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>());

  AdditionalDataContents additional_data{{1.0, 1.0, 1.0},
                                         Vector(1.0, 0.0, 0.0)};

  Geometry::ComputeShadingNormalResult front_normal =
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

  Geometry::ComputeShadingNormalResult back_normal =
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

TEST(Triangle, ComputeShadingNormalFromMap) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Geometry::ComputeShadingNormalResult front_normal =
      triangle->ComputeShadingNormal(FRONT_FACE, nullptr);
  EXPECT_FALSE(front_normal.surface_normal);
  ASSERT_TRUE(front_normal.dp_duv);
  EXPECT_EQ(1.0, front_normal.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal.dp_duv->second.z);
  EXPECT_EQ(front_normal_map.Get(), front_normal.normal_map);

  Geometry::ComputeShadingNormalResult back_normal =
      triangle->ComputeShadingNormal(BACK_FACE, nullptr);
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

TEST(Triangle, ComputeShadingNormalFromNormals) {
  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      {{Point(0.0, 0.0, 0.0), Point(1.0, 0.0, 0.0), Point(0.0, 1.0, 0.0)}},
      {{{0, 1, 2}}}, {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}},
      {{{static_cast<geometric>(0.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(1.0), static_cast<geometric>(0.0)},
        {static_cast<geometric>(0.0), static_cast<geometric>(1.0)}}},
      ReferenceCounted<textures::ValueTexture2D<bool>>(), back_material,
      front_material, front_emissive_material, back_emissive_material,
      ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>());

  AdditionalDataContents additional_data0_front{{1.0, 0.0, 0.0},
                                                Vector(1.0, 0.0, 0.0)};
  Geometry::ComputeShadingNormalResult front_normal0 =
      triangles.front()->ComputeShadingNormal(FRONT_FACE,
                                              &additional_data0_front);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), front_normal0.surface_normal);
  ASSERT_TRUE(front_normal0.dp_duv);
  EXPECT_EQ(1.0, front_normal0.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal0.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal0.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal0.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal0.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal0.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal0.normal_map);

  AdditionalDataContents additional_data0_back{{1.0, 0.0, 0.0},
                                               Vector(-1.0, 0.0, 0.0)};
  Geometry::ComputeShadingNormalResult back_normal0 =
      triangles.front()->ComputeShadingNormal(BACK_FACE,
                                              &additional_data0_back);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0), back_normal0.surface_normal);
  ASSERT_TRUE(back_normal0.dp_duv);
  EXPECT_EQ(1.0, back_normal0.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal0.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal0.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal0.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal0.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal0.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal0.normal_map);

  AdditionalDataContents additional_data1_front{{0.0, 1.0, 0.0},
                                                Vector(0.0, 1.0, 0.0)};
  Geometry::ComputeShadingNormalResult front_normal1 =
      triangles.front()->ComputeShadingNormal(FRONT_FACE,
                                              &additional_data1_front);
  EXPECT_EQ(Vector(0.0, 1.0, 0.0), front_normal1.surface_normal);
  ASSERT_TRUE(front_normal1.dp_duv);
  EXPECT_EQ(1.0, front_normal1.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal1.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal1.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal1.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal1.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal1.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal1.normal_map);

  AdditionalDataContents additional_data1_back{{0.0, 1.0, 0.0},
                                               Vector(0.0, -1.0, 0.0)};
  Geometry::ComputeShadingNormalResult back_normal1 =
      triangles.front()->ComputeShadingNormal(BACK_FACE,
                                              &additional_data1_back);
  EXPECT_EQ(Vector(0.0, -1.0, 0.0), back_normal1.surface_normal);
  ASSERT_TRUE(back_normal1.dp_duv);
  EXPECT_EQ(1.0, back_normal1.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal1.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal1.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal1.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal1.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal1.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal1.normal_map);

  AdditionalDataContents additional_data2_front{{0.0, 0.0, 1.0},
                                                Vector(0.0, 0.0, 1.0)};
  Geometry::ComputeShadingNormalResult front_normal2 =
      triangles.front()->ComputeShadingNormal(FRONT_FACE,
                                              &additional_data2_front);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), front_normal2.surface_normal);
  ASSERT_TRUE(front_normal2.dp_duv);
  EXPECT_EQ(1.0, front_normal2.dp_duv->first.x);
  EXPECT_EQ(0.0, front_normal2.dp_duv->first.y);
  EXPECT_EQ(0.0, front_normal2.dp_duv->first.z);
  EXPECT_EQ(0.0, front_normal2.dp_duv->second.x);
  EXPECT_EQ(1.0, front_normal2.dp_duv->second.y);
  EXPECT_EQ(0.0, front_normal2.dp_duv->second.z);
  EXPECT_EQ(nullptr, front_normal2.normal_map);

  AdditionalDataContents additional_data2_back{{0.0, 0.0, 1.0},
                                               Vector(0.0, 0.0, -1.0)};
  Geometry::ComputeShadingNormalResult back_normal2 =
      triangles.front()->ComputeShadingNormal(BACK_FACE,
                                              &additional_data2_back);
  EXPECT_EQ(Vector(0.0, 0.0, -1.0), back_normal2.surface_normal);
  ASSERT_TRUE(back_normal2.dp_duv);
  EXPECT_EQ(1.0, back_normal2.dp_duv->first.x);
  EXPECT_EQ(0.0, back_normal2.dp_duv->first.y);
  EXPECT_EQ(0.0, back_normal2.dp_duv->first.z);
  EXPECT_EQ(0.0, back_normal2.dp_duv->second.x);
  EXPECT_EQ(1.0, back_normal2.dp_duv->second.y);
  EXPECT_EQ(0.0, back_normal2.dp_duv->second.z);
  EXPECT_EQ(nullptr, back_normal2.normal_map);
}

TEST(Triangle, ComputeHitPoint) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  Point origin(1.0, 0.25, 0.25);
  Vector direction(-1.0, 0.0, 0.0);
  Ray ray(origin, direction);

  AdditionalDataContents additional_data0{{0.0, 0.0, 1.0},
                                          Vector(0.0, 0.0, -1.0)};
  Geometry::ComputeHitPointResult hit_point0 =
      triangle->ComputeHitPoint(ray, 1.0, &additional_data0);
  EXPECT_EQ(hit_point0.point, Point(0.0, 1.0, 0.0));
  EXPECT_EQ(hit_point0.error.x, 0.0);
  EXPECT_LE(std::abs(hit_point0.error.y), 1.0 * RoundingError(7));
  EXPECT_EQ(hit_point0.error.z, 0.0);
}

TEST(Triangle, GetMaterial) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  const Material* front = triangle->GetMaterial(FRONT_FACE);
  EXPECT_EQ(front_material.Get(), front);

  const Material* back = triangle->GetMaterial(BACK_FACE);
  EXPECT_EQ(back_material.Get(), back);
}

TEST(Triangle, GetEmissiveMaterial) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  const EmissiveMaterial* front = triangle->GetEmissiveMaterial(FRONT_FACE);
  EXPECT_EQ(front_emissive_material.Get(), front);

  const EmissiveMaterial* back = triangle->GetEmissiveMaterial(BACK_FACE);
  EXPECT_EQ(back_emissive_material.Get(), back);
}

TEST(Triangle, ComputeSurfaceArea) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  visual_t surface_area0 = triangle->ComputeSurfaceArea(FRONT_FACE, nullptr);
  EXPECT_EQ(0.5, surface_area0);

  Matrix model_to_world = Matrix::Scalar(2.0, 2.0, 2.0).value();
  visual_t surface_area1 =
      triangle->ComputeSurfaceArea(FRONT_FACE, &model_to_world);
  EXPECT_EQ(2.0, surface_area1);
}

TEST(Triangle, SampleBySolidAngle) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();

  MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillRepeatedly(Return(0.0));

  Sampler sampler0(rng0);
  std::variant<std::monostate, Point, Vector> sample0 =
      triangle->SampleBySolidAngle(Point(-1.0, -1.0, -1.0), FRONT_FACE,
                                   sampler0);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), std::get<Point>(sample0));

  MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillRepeatedly(Return(1.0));

  Sampler sampler1(rng1);
  std::variant<std::monostate, Point, Vector> sample1 =
      triangle->SampleBySolidAngle(Point(-1.0, -1.0, -1.0), FRONT_FACE,
                                   sampler1);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), std::get<Point>(sample1));

  MockRandom rng2;
  {
    InSequence s;
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(Return(1.0));
    EXPECT_CALL(rng2, NextGeometric()).WillOnce(Return(0.0));
  }

  Sampler sampler2(rng2);
  std::variant<std::monostate, Point, Vector> sample2 =
      triangle->SampleBySolidAngle(Point(-1.0, -1.0, -1.0), FRONT_FACE,
                                   sampler2);
  EXPECT_EQ(Point(1.0, 0.0, 0.0), std::get<Point>(sample2));

  MockRandom rng3;
  {
    InSequence s;
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(Return(0.0));
    EXPECT_CALL(rng3, NextGeometric()).WillOnce(Return(1.0));
  }

  Sampler sampler3(rng3);
  std::variant<std::monostate, Point, Vector> sample3 =
      triangle->SampleBySolidAngle(Point(-1.0, -1.0, -1.0), FRONT_FACE,
                                   sampler3);
  EXPECT_EQ(Point(0.0, 1.0, 0.0), std::get<Point>(sample3));

  MockRandom rng4;
  EXPECT_CALL(rng4, NextGeometric()).WillRepeatedly(Return(0.25));

  Sampler sampler4(rng4);
  std::variant<std::monostate, Point, Vector> sample4 =
      triangle->SampleBySolidAngle(Point(-1.0, -1.0, -1.0), FRONT_FACE,
                                   sampler4);
  EXPECT_EQ(Point(0.25, 0.25, 0.0), std::get<Point>(sample4));

  MockRandom rng5;
  EXPECT_CALL(rng5, NextGeometric()).WillRepeatedly(Return(0.75));

  Sampler sampler5(rng5);
  std::variant<std::monostate, Point, Vector> sample5 =
      triangle->SampleBySolidAngle(Point(-1.0, -1.0, -1.0), FRONT_FACE,
                                   sampler5);
  EXPECT_EQ(Point(0.25, 0.25, 0.0), std::get<Point>(sample4));
}

TEST(Triangle, ComputePdfBySolidAngle) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();
  AdditionalDataContents additional_data{{0.0, 0.0, 1.0},
                                         Vector(0.0, 0.0, 1.0)};

  std::optional<visual_t> pdf0 = triangle->ComputePdfBySolidAngle(
      Point(0.0, 0.0, 1.0), FRONT_FACE, &additional_data, Point(0.0, 0.0, 0.0));
  EXPECT_NEAR(2.0, pdf0.value(), 0.001);

  std::optional<visual_t> pdf1 = triangle->ComputePdfBySolidAngle(
      Point(0.0, 0.0, 1.0), FRONT_FACE, &additional_data, Point(1.0, 0.0, 0.0));
  EXPECT_NEAR(5.6568541, pdf1.value(), 0.001);

  std::optional<visual_t> pdf2 = triangle->ComputePdfBySolidAngle(
      Point(0.0, 0.0, 1.0), FRONT_FACE, &additional_data,
      Point(0.25, 0.25, 0.0));
  EXPECT_NEAR(2.3864853, pdf2.value(), 0.001);
}

TEST(Triangle, GetBounds) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();
  BoundingBox bounds = triangle->ComputeBounds(nullptr);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), bounds.lower);
  EXPECT_EQ(Point(1.0, 1.0, 0.0), bounds.upper);
}

TEST(Triangle, GetBoundsWithTransform) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();
  Matrix transform = Matrix::Scalar(2.0, 2.0, 2.0).value();
  BoundingBox bounds = triangle->ComputeBounds(&transform);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), bounds.lower);
  EXPECT_EQ(Point(2.0, 2.0, 0.0), bounds.upper);
}

TEST(Triangle, GetFaces) {
  ReferenceCounted<Geometry> triangle = MakeSimpleTriangle();
  std::span<const face_t> faces = triangle->GetFaces();
  ASSERT_EQ(2u, faces.size());
  EXPECT_EQ(FRONT_FACE, faces[0]);
  EXPECT_EQ(BACK_FACE, faces[1]);
}

}  // namespace
}  // namespace geometry
}  // namespace iris
