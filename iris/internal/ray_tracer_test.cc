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

TEST(RayTracerTest, NoGeometry) {
  RayTracer ray_tracer;
  SceneObjects scene_objects = SceneObjects::Builder().Build();
  std::unique_ptr<Scene> scene =
      ListScene::Builder::Create()->Build(scene_objects);
  EXPECT_EQ(nullptr,
            ray_tracer.Trace(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0)),
                             0.0, 1.0, *scene));
}

TEST(RayTracerTest, WithGeometry) {
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
  const Hit* hit = ray_tracer.Trace(ray, 0.0, 2.0, *scene);
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

}  // namespace
}  // namespace internal
}  // namespace iris