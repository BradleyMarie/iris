#include "iris/geometry.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/internal/hit_arena.h"
#include "iris/random/mock_random.h"

TEST(GeometryTest, Trace) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto arena = iris::internal::HitArena();
  iris::HitAllocator allocator(ray, arena);

  iris::geometry::MockBasicGeometry nested_geom;
  EXPECT_CALL(nested_geom, Trace(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Ray& trace_ray, iris::HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            return &hit_allocator.Allocate(
                nullptr, 2.0, static_cast<iris::geometric_t>(0.0), 3, 4);
          }));

  iris::geometry::MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Ray& trace_ray, iris::HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            const iris::Geometry* base_geom = &nested_geom;
            auto* nested_hit = base_geom->Trace(hit_allocator);
            return &hit_allocator.Allocate(
                nested_hit, 1.0, static_cast<iris::geometric_t>(0.0), 2, 3);
          }));

  iris::Geometry* base_geom = &geom;
  auto* hit = base_geom->Trace(allocator);

  auto* full_hit = static_cast<iris::internal::Hit*>(hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(base_geom, full_hit->geometry);

  ASSERT_NE(nullptr, hit->next);
  auto* next_hit = hit->next;
  auto* next_full_hit = static_cast<iris::internal::Hit*>(next_hit);
  EXPECT_EQ(2.0, next_hit->distance);
  EXPECT_EQ(3u, next_full_hit->front);
  EXPECT_EQ(4u, next_full_hit->back);
  EXPECT_EQ(&nested_geom, next_full_hit->geometry);
};

TEST(GeometryTest, ComputeTextureCoordinates) {
  iris::geometry::MockBasicGeometry geom;
  EXPECT_FALSE(geom.ComputeTextureCoordinates(iris::Point(0.0, 0.0, 0.0),
                                              std::nullopt, 0, nullptr)
                   .has_value());
}

TEST(GeometryTest, ComputeShadingNormal) {
  iris::geometry::MockBasicGeometry geom;
  auto shading_normal = geom.ComputeShadingNormal(0, nullptr);
  EXPECT_FALSE(shading_normal.surface_normal);
  EXPECT_FALSE(shading_normal.dp_duv);
  EXPECT_EQ(nullptr, shading_normal.normal_map);
}

TEST(GeometryTest, GetMaterial) {
  iris::geometry::MockBasicGeometry geom;
  EXPECT_EQ(nullptr, geom.GetMaterial(0));
}

TEST(GeometryTest, GetEmissiveMaterial) {
  iris::geometry::MockBasicGeometry geom;
  EXPECT_EQ(nullptr, geom.GetEmissiveMaterial(0));
}

TEST(GeometryTest, SampleBySolidAngle) {
  iris::geometry::MockBasicGeometry geom;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);
  EXPECT_TRUE(std::holds_alternative<std::monostate>(
      geom.SampleBySolidAngle(iris::Point(0.0, 0.0, 0.0), 0, sampler)));
}

TEST(GeometryTest, ComputePdfBySolidAngle) {
  iris::geometry::MockBasicGeometry geom;
  EXPECT_FALSE(geom.ComputePdfBySolidAngle(
      iris::Point(0.0, 0.0, 0.0), 0, nullptr, iris::Point(0.0, 0.0, 0.0)));
}