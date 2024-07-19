#include "iris/scenes/list_scene.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/reference_counted.h"
#include "iris/scene_objects.h"
#include "iris/testing/intersector.h"

static const iris::face_t kFaces[] = {0u};

TEST(ListScene, Empty) {
  iris::SceneObjects::Builder scene_objects_builder;
  auto scene_objects = scene_objects_builder.Build();

  iris::Ray ray(iris::Point(0.25, 0.25, -5.5), iris::Vector(0.0, 0.0, -1.0));
  iris::Hit* closest_hit = nullptr;
  auto intersector =
      iris::testing::MakeIntersector(ray, 0.0, 100.0, closest_hit);

  auto scene =
      iris::scenes::ListScene::Builder().Create()->Build(scene_objects);
  scene->Trace(ray, intersector);
  EXPECT_EQ(nullptr, closest_hit);
}

TEST(ListScene, TestsAll) {
  auto mock_geometry0 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(0.5, 0.5, 1.0));
  EXPECT_CALL(*mock_geometry0, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry0, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry0, ComputeBounds(nullptr))
      .WillRepeatedly(testing::Return(bounds0));

  auto mock_geometry1 =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 3.0),
                            iris::Point(2.5, 2.5, 4.0));
  EXPECT_CALL(*mock_geometry1, GetFaces())
      .WillRepeatedly(testing::Return(std::span{kFaces}));
  EXPECT_CALL(*mock_geometry1, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*mock_geometry1, ComputeBounds(nullptr))
      .WillRepeatedly(testing::Return(bounds1));

  iris::SceneObjects::Builder scene_objects_builder;
  scene_objects_builder.Add(mock_geometry0);
  scene_objects_builder.Add(mock_geometry1);
  auto scene_objects = scene_objects_builder.Build();

  EXPECT_CALL(*mock_geometry0, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*mock_geometry1, Trace(testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Return(nullptr));

  iris::Ray ray(iris::Point(0.25, 0.25, -5.5), iris::Vector(0.0, 0.0, -1.0));
  iris::Hit* closest_hit = nullptr;
  auto intersector =
      iris::testing::MakeIntersector(ray, 0.0, 100.0, closest_hit);

  auto scene =
      iris::scenes::ListScene::Builder().Create()->Build(scene_objects);
  scene->Trace(ray, intersector);
}