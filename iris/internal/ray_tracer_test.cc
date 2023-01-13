#include "iris/internal/ray_tracer.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/scene_objects.h"
#include "iris/scenes/list_scene.h"

TEST(RayTracerTest, NoGeometry) {
  iris::internal::RayTracer ray_tracer;
  auto scene_objects = iris::SceneObjects::Builder().Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(scene_objects);
  EXPECT_EQ(nullptr, ray_tracer.Trace(iris::Ray(iris::Point(0.0, 0.0, 0.0),
                                                iris::Vector(1.0, 1.0, 1.0)),
                                      0.0, 1.0, *scene));
}

TEST(RayTracerTest, WithGeometry) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto geometry =
      iris::MakeReferenceCounted<iris::geometry::MockBasicGeometry>();
  EXPECT_CALL(*geometry, Trace(ray, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Ray& trace_ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 2, 3);
          }));

  auto* geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));
  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder().Build(objects);

  iris::internal::RayTracer ray_tracer;
  auto* hit = ray_tracer.Trace(ray, 0.0, 2.0, *scene);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(nullptr, hit->next);
  EXPECT_EQ(2u, hit->front);
  EXPECT_EQ(3u, hit->back);
  EXPECT_EQ(geometry_ptr, hit->geometry);
  EXPECT_EQ(nullptr, hit->model_to_world);
  EXPECT_EQ(nullptr, hit->additional_data);
}