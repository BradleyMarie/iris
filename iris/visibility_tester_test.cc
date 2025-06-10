#include "iris/visibility_tester.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/hit_allocator.h"
#include "iris/internal/ray_tracer.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"
#include "iris/scenes/list_scene.h"
#include "iris/vector.h"

namespace iris {
namespace {

using ::iris::geometry::MockBasicGeometry;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

TEST(VisibilityTesterTest, NoGeometry) {
  internal::RayTracer ray_tracer;
  internal::Arena arena;
  SceneObjects objects = SceneObjects::Builder().Build();
  std::unique_ptr<Scene> scene = scenes::MakeListSceneBuilder()->Build(objects);
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_TRUE(visibility_tester.Visible(
      Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_TRUE(visibility_tester.Visible(
      Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0)), 1.0));
}

TEST(VisibilityTesterTest, WithGeometry) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, Trace(ray, _))
      .WillRepeatedly(
          Invoke([&](const Ray& trace_ray, HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0,
                                           static_cast<geometric_t>(0.0), 2, 3);
          }));

  SceneObjects::Builder builder;
  builder.Add(geometry);
  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = scenes::MakeListSceneBuilder()->Build(objects);

  internal::RayTracer ray_tracer;
  internal::Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_TRUE(visibility_tester.Visible(
      Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0)), 0.5));
  EXPECT_FALSE(visibility_tester.Visible(
      Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0)), 1.5));
  EXPECT_FALSE(visibility_tester.Visible(
      Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
}

}  // namespace
}  // namespace iris
