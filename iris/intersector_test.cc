#include "iris/intersector.h"

#include <limits>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/internal/hit_arena.h"
#include "iris/matrix.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace {

using ::iris::geometry::MockBasicGeometry;
using ::testing::_;
using ::testing::Invoke;

static const Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));

std::unique_ptr<MockBasicGeometry> MakeMockGeometry(
    geometric_t distance, geometric_t error = 0.0,
    const Ray& transformed_ray = ray) {
  std::unique_ptr<MockBasicGeometry> result =
      std::make_unique<MockBasicGeometry>();
  EXPECT_CALL(*result, Trace(transformed_ray, _))
      .WillRepeatedly(Invoke(
          [distance, error](const Ray& trace_ray, HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, distance, error, 2, 3);
          }));
  return result;
}

TEST(InspectorTest, Initialize) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TooClose) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(0.0);
  EXPECT_FALSE(intersector.Intersect(*geometry));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TooCloseAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(0.5, 0.5);
  EXPECT_FALSE(intersector.Intersect(*geometry));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TooFar) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(3.0);
  EXPECT_FALSE(intersector.Intersect(*geometry));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TooFarAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(1.5, 0.5);
  EXPECT_FALSE(intersector.Intersect(*geometry));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, Hits) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(1.0, 0.5);
  EXPECT_FALSE(intersector.Intersect(*geometry));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, HitsAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(1.0, 0.5);
  EXPECT_TRUE(intersector.Intersect(*geometry));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, HitsIgnoreFarther) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 = MakeMockGeometry(1.0);
  EXPECT_FALSE(intersector.Intersect(*geometry0));

  std::unique_ptr<MockBasicGeometry> geometry1 = MakeMockGeometry(2.0);
  EXPECT_FALSE(intersector.Intersect(*geometry1));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, KeepsCloser) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 = MakeMockGeometry(2.0);
  EXPECT_FALSE(intersector.Intersect(*geometry0));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 = MakeMockGeometry(1.0);
  EXPECT_FALSE(intersector.Intersect(*geometry1));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, KeepsCloserAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry0 = MakeMockGeometry(2.0);
  EXPECT_TRUE(intersector.Intersect(*geometry0));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 = MakeMockGeometry(1.0);
  EXPECT_TRUE(intersector.Intersect(*geometry1));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

const static Matrix model_to_world = Matrix::Translation(1.0, 1.0, 1.0).value();

TEST(IntersectorTest, TransformedTooClose) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(0.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TransformedTooCloseAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(0.5, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TransformedTooFar) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(3.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TransformedTooFarAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(1.5, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TransformedHits) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(1.0, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, TransformedHitsAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(1.0, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_TRUE(intersector.Intersect(*geometry, model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, TransformedHitsIgnoreFarther) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 =
      MakeMockGeometry(1.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry0, model_to_world));

  std::unique_ptr<MockBasicGeometry> geometry1 =
      MakeMockGeometry(2.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry1, model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, TransformedKeepsCloser) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 =
      MakeMockGeometry(2.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry0, model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 =
      MakeMockGeometry(1.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry1, model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, TransformedKeepsCloserAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry0 =
      MakeMockGeometry(2.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_TRUE(intersector.Intersect(*geometry0, model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 =
      MakeMockGeometry(1.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_TRUE(intersector.Intersect(*geometry1, model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerTooClose) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(0.0);
  EXPECT_FALSE(intersector.Intersect(*geometry, nullptr));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerTooCloseAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(0.5, 0.5);
  EXPECT_FALSE(intersector.Intersect(*geometry, nullptr));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerTooFar) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(3.0);
  EXPECT_FALSE(intersector.Intersect(*geometry, nullptr));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerTooFarAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(1.5, 0.5);
  EXPECT_FALSE(intersector.Intersect(*geometry, nullptr));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerHits) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(1.0, 0.5);
  EXPECT_FALSE(intersector.Intersect(*geometry, nullptr));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerHitsAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry = MakeMockGeometry(1.0, 0.5);
  EXPECT_TRUE(intersector.Intersect(*geometry, nullptr));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerHitsIgnoreFarther) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 = MakeMockGeometry(1.0);
  EXPECT_FALSE(intersector.Intersect(*geometry0, nullptr));

  std::unique_ptr<MockBasicGeometry> geometry1 = MakeMockGeometry(2.0);
  EXPECT_FALSE(intersector.Intersect(*geometry1, nullptr));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerKeepsCloser) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 = MakeMockGeometry(2.0);
  EXPECT_FALSE(intersector.Intersect(*geometry0));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 = MakeMockGeometry(1.0);
  EXPECT_FALSE(intersector.Intersect(*geometry1, nullptr));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerKeepsCloserAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry0 = MakeMockGeometry(2.0);
  EXPECT_TRUE(intersector.Intersect(*geometry0));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 = MakeMockGeometry(1.0);
  EXPECT_TRUE(intersector.Intersect(*geometry1, nullptr));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
  EXPECT_EQ(ray, full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerTransformedTooClose) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(0.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, &model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerTransformedTooCloseAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(0.5, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, &model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerTransformedTooFar) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(3.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, &model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerTransformedTooFarAfterError) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 1.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(1.5, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, &model_to_world));

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, MatrixPointerTransformedHits) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(1.0, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry, &model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerTransformedHitsAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry =
      MakeMockGeometry(1.0, 0.5, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_TRUE(intersector.Intersect(*geometry, &model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.5, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerTransformedHitsIgnoreFarther) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 2.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 =
      MakeMockGeometry(1.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry0, &model_to_world));

  std::unique_ptr<MockBasicGeometry> geometry1 =
      MakeMockGeometry(2.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry1, &model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerTransformedKeepsCloser) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/true);

  std::unique_ptr<MockBasicGeometry> geometry0 =
      MakeMockGeometry(2.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry0, &model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 =
      MakeMockGeometry(1.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_FALSE(intersector.Intersect(*geometry1, &model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

TEST(IntersectorTest, MatrixPointerTransformedKeepsCloserAny) {
  internal::HitArena arena;

  Hit* closest_hit = nullptr;
  Intersector intersector(ray, 0.0, 3.0, arena, closest_hit,
                          /*find_closest_hit=*/false);

  std::unique_ptr<MockBasicGeometry> geometry0 =
      MakeMockGeometry(2.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_TRUE(intersector.Intersect(*geometry0, &model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  std::unique_ptr<MockBasicGeometry> geometry1 =
      MakeMockGeometry(1.0, 0.0, model_to_world.InverseMultiplyWithError(ray));
  EXPECT_TRUE(intersector.Intersect(*geometry1, &model_to_world));

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  internal::Hit* full_hit = static_cast<internal::Hit*>(closest_hit);
  ASSERT_NE(full_hit, nullptr);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(0.0, full_hit->distance_error);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
  EXPECT_EQ(model_to_world.InverseMultiplyWithError(ray),
            full_hit->model_ray.value());
}

}  // namespace
}  // namespace iris
