#include "iris/visibility_tester.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/internal/ray_tracer.h"
#include "iris/scenes/list_scene.h"

TEST(VisibilityTesterTest, NoGeometry) {
  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  auto objects = iris::SceneObjects::Builder().Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);
  iris::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_TRUE(visibility_tester.Visible(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_TRUE(visibility_tester.Visible(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0)), 1.0));
}

TEST(VisibilityTesterTest, WithGeometry) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto geometry =
      iris::MakeReferenceCounted<iris::geometry::MockBasicGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, Trace(ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [&](const iris::Ray& trace_ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 2, 3);
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));
  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_TRUE(visibility_tester.Visible(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0)), 0.5));
  EXPECT_FALSE(visibility_tester.Visible(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0)), 1.5));
  EXPECT_FALSE(visibility_tester.Visible(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
}