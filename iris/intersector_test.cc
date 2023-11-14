#include "iris/intersector.h"

#include <limits>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/internal/hit_arena.h"

static const iris::Ray ray(iris::Point(0.0, 0.0, 0.0),
                           iris::Vector(1.0, 1.0, 1.0));

std::unique_ptr<iris::geometry::MockBasicGeometry> MakeMockGeometry(
    iris::geometric distance) {
  auto result = std::make_unique<iris::geometry::MockBasicGeometry>();
  EXPECT_CALL(*result, Trace(ray, testing::_))
      .WillRepeatedly(
          testing::Invoke([distance](const iris::Ray& trace_ray,
                                     iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, distance, 2, 3);
          }));
  return result;
}

TEST(InspectorTest, Initialize) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 1.0, 2.0, arena, closest_hit);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
}

TEST(IntersectorTest, TooClose) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(0.0);
  intersector.Intersect(*geometry);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, TooFar) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(3.0);
  intersector.Intersect(*geometry);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, Hits) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 0.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
};

TEST(IntersectorTest, HitsIgnoreFarther) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 0.0, 2.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry0);

  auto geometry1 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry1);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
};

TEST(IntersectorTest, KeepsCloser) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 0.0, 3.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry0);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  auto geometry1 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry1);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
};

static const iris::Matrix model_to_world =
    iris::Matrix::Translation(1.0, 1.0, 1.0).value();
static const iris::Ray transformed_ray(iris::Point(1.0, 1.0, 1.0),
                                       iris::Vector(1.0, 1.0, 1.0));

TEST(IntersectorTest, TransformedTooClose) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(0.0);
  intersector.Intersect(*geometry, model_to_world);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, TransformedTooFar) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(3.0);
  intersector.Intersect(*geometry, model_to_world);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, TransformedHits) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 0.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry, model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
};

TEST(IntersectorTest, TransformedHitsIgnoreFarther) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 0.0, 2.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry0, model_to_world);

  auto geometry1 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry1, model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
};

TEST(IntersectorTest, TransformedKeepsCloser) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 0.0, 3.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry0, model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  auto geometry1 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry1, model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
};

TEST(IntersectorTest, MatrixPointerTooClose) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(0.0);
  intersector.Intersect(*geometry, nullptr);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, MatrixPointerTooFar) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(3.0);
  intersector.Intersect(*geometry, nullptr);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, MatrixPointerHits) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 0.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry, nullptr);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
};

TEST(IntersectorTest, MatrixPointerHitsIgnoreFarther) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 0.0, 2.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry0, nullptr);

  auto geometry1 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry1, nullptr);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
};

TEST(IntersectorTest, MatrixPointerKeepsCloser) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(ray, 0.0, 3.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry0);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  auto geometry1 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry1, nullptr);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(nullptr, full_hit->model_to_world);
};

TEST(IntersectorTest, MatrixPointerTransformedTooClose) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(0.0);
  intersector.Intersect(*geometry, &model_to_world);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, MatrixPointerTransformedTooFar) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 1.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(3.0);
  intersector.Intersect(*geometry, &model_to_world);

  EXPECT_EQ(nullptr, closest_hit);
  EXPECT_EQ(1.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());
};

TEST(IntersectorTest, MatrixPointerTransformedHits) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 0.0, 2.0, arena, closest_hit);

  auto geometry = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry, &model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
};

TEST(IntersectorTest, MatrixPointerTransformedHitsIgnoreFarther) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 0.0, 2.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry0, &model_to_world);

  auto geometry1 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry1, &model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry0.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
};

TEST(IntersectorTest, MatrixPointerTransformedKeepsCloser) {
  auto arena = iris::internal::HitArena();

  iris::Hit* closest_hit;
  iris::Intersector intersector(transformed_ray, 0.0, 3.0, arena, closest_hit);

  auto geometry0 = MakeMockGeometry(2.0);
  intersector.Intersect(*geometry0, &model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(2.0, intersector.MaximumDistance());

  auto geometry1 = MakeMockGeometry(1.0);
  intersector.Intersect(*geometry1, &model_to_world);

  EXPECT_EQ(0.0, intersector.MinimumDistance());
  EXPECT_EQ(1.0, intersector.MaximumDistance());

  auto* full_hit = static_cast<iris::internal::Hit*>(closest_hit);
  EXPECT_EQ(1.0, closest_hit->distance);
  EXPECT_EQ(2u, full_hit->front);
  EXPECT_EQ(3u, full_hit->back);
  EXPECT_EQ(geometry1.get(), full_hit->geometry);
  EXPECT_EQ(&model_to_world, full_hit->model_to_world);
};