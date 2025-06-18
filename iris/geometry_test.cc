#include "iris/geometry.h"

#include <limits>
#include <optional>
#include <variant>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/internal/hit_arena.h"
#include "iris/point.h"
#include "iris/random/mock_random.h"
#include "iris/ray.h"
#include "iris/sampler.h"
#include "iris/vector.h"

namespace iris {
namespace {

using ::iris::geometry::MockBasicGeometry;
using ::iris::random::MockRandom;
using ::testing::_;
using ::testing::Invoke;

TEST(GeometryTest, TraceAllHits) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry nested_geom;
  EXPECT_CALL(nested_geom,
              Trace(_, -std::numeric_limits<geometric_t>::infinity(),
                    std::numeric_limits<geometric_t>::infinity(),
                    Geometry::ALL_HITS, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            return &hit_allocator.Allocate(nullptr, 2.0,
                                           static_cast<geometric_t>(0.0), 3, 4);
          }));

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, -std::numeric_limits<geometric_t>::infinity(),
                          std::numeric_limits<geometric_t>::infinity(),
                          Geometry::ALL_HITS, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            const Geometry* base_geom = &nested_geom;
            Hit* nested_hit = base_geom->TraceAllHits(hit_allocator);
            return &hit_allocator.Allocate(nested_hit, 1.0,
                                           static_cast<geometric_t>(0.0), 2, 3);
          }));

  Geometry* base_geom = &geom;
  Hit* hit = base_geom->TraceAllHits(allocator);

  internal::Hit* full_hit = static_cast<internal::Hit*>(hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(base_geom, full_hit->geometry);

  ASSERT_NE(nullptr, hit->next);
  Hit* next_hit = hit->next;
  internal::Hit* next_full_hit = static_cast<internal::Hit*>(next_hit);
  EXPECT_EQ(2.0, next_hit->distance);
  EXPECT_EQ(3u, next_full_hit->front);
  EXPECT_EQ(4u, next_full_hit->back);
  EXPECT_EQ(&nested_geom, next_full_hit->geometry);
};

TEST(GeometryTest, TraceOneHitIgnoresTooClose) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, 3.0, 4.0, Geometry::ANY_HIT, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            return &hit_allocator.Allocate(nullptr, 2.0,
                                           static_cast<geometric_t>(0.0), 3, 4);
          }));

  EXPECT_EQ(nullptr, geom.TraceOneHit(allocator, 3.0, 4.0, false));
};

TEST(GeometryTest, TraceOneHitIgnoresTooCloseAfterError) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, 3.0, 10.0, Geometry::ANY_HIT, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            return &hit_allocator.Allocate(nullptr, 4.0,
                                           static_cast<geometric_t>(1.0), 3, 4);
          }));

  EXPECT_EQ(nullptr, geom.TraceOneHit(allocator, 3.0, 10.0, false));
};

TEST(GeometryTest, TraceOneHitIgnoresTooFar) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, 3.0, 4.0, Geometry::ANY_HIT, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            return &hit_allocator.Allocate(nullptr, 6.0,
                                           static_cast<geometric_t>(0.0), 3, 4);
          }));

  EXPECT_EQ(nullptr, geom.TraceOneHit(allocator, 3.0, 4.0, false));
};

TEST(GeometryTest, TraceOneHitIgnoresTooFarAfterError) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, 3.0, 5.0, Geometry::ANY_HIT, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            return &hit_allocator.Allocate(nullptr, 4.0,
                                           static_cast<geometric_t>(1.0), 3, 4);
          }));

  EXPECT_EQ(nullptr, geom.TraceOneHit(allocator, 3.0, 5.0, false));
};

TEST(GeometryTest, TraceOneHitFindsClosest) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, 0.0, 10.0, Geometry::CLOSEST_HIT, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            Hit* closest_hit = &hit_allocator.Allocate(
                nullptr, 3.0, static_cast<geometric_t>(0.0), 3, 4);
            return &hit_allocator.Allocate(closest_hit, 4.0,
                                           static_cast<geometric_t>(0.0), 3, 4);
          }));

  Hit* hit = geom.TraceOneHit(allocator, 0.0, 10.0, true);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(3.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);
};

TEST(GeometryTest, TraceOneHitFindsAny) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, 0.0, 10.0, Geometry::ANY_HIT, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            Hit* last_hit = &hit_allocator.Allocate(
                nullptr, 10.0, static_cast<geometric_t>(0.0), 3, 4);
            Hit* closest_hit = &hit_allocator.Allocate(
                last_hit, 3.0, static_cast<geometric_t>(0.0), 3, 4);
            return &hit_allocator.Allocate(closest_hit, 4.0,
                                           static_cast<geometric_t>(0.0), 3, 4);
          }));

  Hit* hit = geom.TraceOneHit(allocator, 0.0, 10.0, false);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(4.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);
};

TEST(GeometryTest, TraceOneHitClosestNotFound) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  internal::HitArena arena;
  HitAllocator allocator(ray, arena);

  MockBasicGeometry geom;
  EXPECT_CALL(geom, Trace(_, 0.0, 10.0, Geometry::CLOSEST_HIT, _))
      .WillOnce(
          ([&](const Ray& trace_ray, geometric_t minimum_distance,
               geometric_t maximum_distance, Geometry::TraceMode trace_mode,
               HitAllocator& hit_allocator) {
            EXPECT_EQ(ray, trace_ray);
            return nullptr;
          }));

  ASSERT_EQ(nullptr, geom.TraceOneHit(allocator, 0.0, 10.0, true));
};

TEST(GeometryTest, ComputeTextureCoordinates) {
  MockBasicGeometry geom;
  EXPECT_FALSE(geom.ComputeTextureCoordinates(Point(0.0, 0.0, 0.0),
                                              std::nullopt, 0, nullptr)
                   .has_value());
}

TEST(GeometryTest, ComputeShadingNormal) {
  MockBasicGeometry geom;
  Geometry::ComputeShadingNormalResult shading_normal =
      geom.ComputeShadingNormal(0, nullptr);
  EXPECT_FALSE(shading_normal.surface_normal);
  EXPECT_FALSE(shading_normal.dp_duv);
  EXPECT_EQ(nullptr, shading_normal.normal_map);
}

TEST(GeometryTest, GetMaterial) {
  MockBasicGeometry geom;
  EXPECT_EQ(nullptr, geom.GetMaterial(0));
}

TEST(GeometryTest, GetEmissiveMaterial) {
  MockBasicGeometry geom;
  EXPECT_EQ(nullptr, geom.GetEmissiveMaterial(0));
}

TEST(GeometryTest, SampleBySolidAngle) {
  MockBasicGeometry geom;
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);
  EXPECT_TRUE(std::holds_alternative<std::monostate>(
      geom.SampleBySolidAngle(Point(0.0, 0.0, 0.0), 0, sampler)));
}

TEST(GeometryTest, ComputePdfBySolidAngle) {
  MockBasicGeometry geom;
  EXPECT_FALSE(geom.ComputePdfBySolidAngle(Point(0.0, 0.0, 0.0), 0, nullptr,
                                           Point(0.0, 0.0, 0.0)));
}

}  // namespace
}  // namespace iris
