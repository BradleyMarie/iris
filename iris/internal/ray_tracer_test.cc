#include "iris/internal/ray_tracer.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/scene_objects.h"
#include "iris/scenes/list_scene.h"

namespace iris {
namespace internal {
namespace {

using ::iris::geometry::MockBasicGeometry;
using ::iris::scenes::ListScene;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

TEST(TraceClosestHit, NoGeometry) {
  RayTracer ray_tracer;
  SceneObjects scene_objects = SceneObjects::Builder().Build();
  std::unique_ptr<Scene> scene =
      ListScene::Builder::Create()->Build(scene_objects);
  EXPECT_EQ(nullptr, ray_tracer.TraceClosestHit(
                         Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0)), 0.0,
                         1.0, *scene));
}

TEST(TraceClosestHit, WithGeometry) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, Trace(ray, _))
      .WillOnce(Invoke([&](const Ray& trace_ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 2, 3);
      }));

  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));
  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder().Build(objects);

  RayTracer ray_tracer;
  const Hit* hit = ray_tracer.TraceClosestHit(ray, 0.0, 2.0, *scene);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(0.0, hit->distance_error);
  EXPECT_EQ(nullptr, hit->next);
  EXPECT_EQ(2u, hit->front);
  EXPECT_EQ(3u, hit->back);
  EXPECT_EQ(geometry_ptr, hit->geometry);
  EXPECT_EQ(nullptr, hit->model_to_world);
  EXPECT_EQ(nullptr, hit->additional_data);
}

TEST(TraceAnyHit, NoGeometry) {
  RayTracer ray_tracer;
  SceneObjects scene_objects = SceneObjects::Builder().Build();
  std::unique_ptr<Scene> scene =
      ListScene::Builder::Create()->Build(scene_objects);
  EXPECT_EQ(nullptr, ray_tracer.TraceAnyHit(
                         Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0)), 0.0,
                         1.0, *scene));
}

TEST(TraceAnyHit, WithGeometry) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, Trace(ray, _))
      .WillOnce(Invoke([&](const Ray& trace_ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 2, 3);
      }));

  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));
  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder().Build(objects);

  RayTracer ray_tracer;
  const Hit* hit = ray_tracer.TraceAnyHit(ray, 0.0, 2.0, *scene);
  ASSERT_NE(nullptr, hit);
  EXPECT_EQ(1.0, hit->distance);
  EXPECT_EQ(0.0, hit->distance_error);
  EXPECT_EQ(nullptr, hit->next);
  EXPECT_EQ(2u, hit->front);
  EXPECT_EQ(3u, hit->back);
  EXPECT_EQ(geometry_ptr, hit->geometry);
  EXPECT_EQ(nullptr, hit->model_to_world);
  EXPECT_EQ(nullptr, hit->additional_data);
}

TEST(TraceBoth, WithGeometry) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, Trace(ray, _))
      .WillRepeatedly(
          Invoke([&](const Ray& trace_ray, HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 2, 3);
          }));

  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));
  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder().Build(objects);

  RayTracer ray_tracer;
  const Hit* hit0 = ray_tracer.TraceClosestHit(ray, 0.0, 2.0, *scene);
  const Hit* hit1 = ray_tracer.TraceAnyHit(ray, 0.0, 2.0, *scene);

  ASSERT_NE(nullptr, hit0);
  EXPECT_EQ(1.0, hit0->distance);
  EXPECT_EQ(0.0, hit0->distance_error);
  EXPECT_EQ(nullptr, hit0->next);
  EXPECT_EQ(2u, hit0->front);
  EXPECT_EQ(3u, hit0->back);
  EXPECT_EQ(geometry_ptr, hit0->geometry);
  EXPECT_EQ(nullptr, hit0->model_to_world);
  EXPECT_EQ(nullptr, hit0->additional_data);

  ASSERT_NE(nullptr, hit1);
  EXPECT_EQ(1.0, hit1->distance);
  EXPECT_EQ(0.0, hit1->distance_error);
  EXPECT_EQ(nullptr, hit1->next);
  EXPECT_EQ(2u, hit1->front);
  EXPECT_EQ(3u, hit1->back);
  EXPECT_EQ(geometry_ptr, hit1->geometry);
  EXPECT_EQ(nullptr, hit1->model_to_world);
  EXPECT_EQ(nullptr, hit1->additional_data);
}

}  // namespace
}  // namespace internal
}  // namespace iris