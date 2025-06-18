#include "iris/scenes/list_scene.h"

#include <memory>
#include <span>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/hit.h"
#include "iris/integer.h"
#include "iris/intersector.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/scene.h"
#include "iris/scene_objects.h"
#include "iris/testing/intersector.h"
#include "iris/vector.h"

namespace iris {
namespace scenes {
namespace {

using ::iris::geometry::MockGeometry;
using ::iris::testing::MakeIntersector;
using ::testing::_;
using ::testing::Return;

static const face_t kFaces[] = {0u};

TEST(ListScene, Empty) {
  SceneObjects::Builder scene_objects_builder;
  SceneObjects scene_objects = scene_objects_builder.Build();

  Ray ray(Point(0.25, 0.25, -5.5), Vector(0.0, 0.0, -1.0));
  Hit* closest_hit = nullptr;
  Intersector intersector = MakeIntersector(ray, 0.0, 100.0, closest_hit);

  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(scene_objects);
  scene->Trace(ray, intersector);
  EXPECT_EQ(nullptr, closest_hit);
}

TEST(ListScene, TestsAll) {
  ReferenceCounted<MockGeometry> mock_geometry0 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds0));

  ReferenceCounted<MockGeometry> mock_geometry1 =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox bounds1(Point(2.0, 2.0, 3.0), Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(Return(bounds1));

  SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  SceneObjects scene_objects = scene_objects_builder.Build();

  EXPECT_CALL(*mock_geometry0, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(_, _, _, _, _))
      .Times(1)
      .WillOnce(Return(nullptr));

  Ray ray(Point(0.25, 0.25, -5.5), Vector(0.0, 0.0, -1.0));
  Hit* closest_hit = nullptr;
  Intersector intersector = MakeIntersector(ray, 0.0, 100.0, closest_hit);

  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(scene_objects);
  scene->Trace(ray, intersector);
}

}  // namespace
}  // namespace scenes
}  // namespace iris
