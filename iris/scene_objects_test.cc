#include "iris/scene_objects.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/lights/mock_light.h"

iris::ReferenceCounted<iris::Geometry> MakeZeroBoundsGeometry() {
  auto result = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*result, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0))));
  return result;
}

iris::ReferenceCounted<iris::Geometry> MakeGeometry(bool emissive) {
  static const std::vector<iris::face_t> faces = {1};
  auto result = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*result, ComputeArea(testing::_))
      .WillRepeatedly(testing::Return(1.0));
  EXPECT_CALL(*result, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(emissive));
  EXPECT_CALL(*result, GetFaces()).WillRepeatedly(testing::Return(faces));
  EXPECT_CALL(*result, ComputeBounds(testing::_))
      .WillRepeatedly(testing::Return(iris::BoundingBox(
          iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 2.0))));
  return result;
}

TEST(SceneObjects, Build) {
  auto geom0 = MakeGeometry(false);
  auto geom1 = MakeGeometry(false);
  auto geom2 = MakeGeometry(false);
  auto geom3 = MakeGeometry(true);
  auto geom4 = MakeZeroBoundsGeometry();
  auto light0 = iris::MakeReferenceCounted<iris::lights::MockLight>();
  auto light1 = iris::MakeReferenceCounted<iris::lights::MockLight>();
  auto environmental_light = iris::MakeReferenceCounted<
      iris::environmental_lights::MockEnvironmentalLight>();
  auto matrix0 = iris::Matrix::Identity();
  auto matrix1 = iris::Matrix::Translation(1.0, 2.0, 3.0).value();
  auto matrix2 = iris::Matrix::Scalar(1.0, 2.0, 3.0).value();

  iris::SceneObjects::Builder builder;

  builder.Add(iris::ReferenceCounted<iris::lights::MockLight>());
  builder.Add(iris::ReferenceCounted<iris::Geometry>(), matrix0);

  builder.Add(light0);
  builder.Add(light1);

  builder.Add(geom0, matrix1);
  builder.Add(geom1, matrix2);
  builder.Add(geom2, matrix2);
  builder.Add(geom3, matrix0);

  builder.Add(geom4);

  builder.Set(environmental_light);

  auto scene_objects = builder.Build();
  ASSERT_EQ(4u, scene_objects.NumGeometry());
  EXPECT_EQ(geom0.Get(), &scene_objects.GetGeometry(0).first);
  EXPECT_EQ(matrix1, *scene_objects.GetGeometry(0).second);
  EXPECT_EQ(geom1.Get(), &scene_objects.GetGeometry(1).first);
  EXPECT_EQ(matrix2, *scene_objects.GetGeometry(1).second);
  EXPECT_EQ(geom2.Get(), &scene_objects.GetGeometry(2).first);
  EXPECT_EQ(matrix2, *scene_objects.GetGeometry(2).second);
  EXPECT_EQ(geom3.Get(), &scene_objects.GetGeometry(3).first);
  EXPECT_EQ(nullptr, scene_objects.GetGeometry(3).second);
  EXPECT_EQ(*scene_objects.GetGeometry(1).second,
            *scene_objects.GetGeometry(2).second);
  ASSERT_EQ(4u, scene_objects.NumLights());
  EXPECT_EQ(light0.Get(), &scene_objects.GetLight(0));
  EXPECT_EQ(light1.Get(), &scene_objects.GetLight(1));
  EXPECT_EQ(environmental_light.Get(), scene_objects.GetEnvironmentalLight());

  // Reuse builder
  geom0 = MakeGeometry(false);
  geom1 = MakeGeometry(false);
  geom2 = MakeGeometry(false);
  geom3 = MakeGeometry(true);
  light0 = iris::MakeReferenceCounted<iris::lights::MockLight>();
  light1 = iris::MakeReferenceCounted<iris::lights::MockLight>();

  builder.Add(light0);
  builder.Add(light1);

  builder.Add(geom0, matrix1);
  builder.Add(geom1, matrix2);
  builder.Add(geom2, matrix2);
  builder.Add(geom3, matrix0);

  scene_objects = builder.Build();
  scene_objects.Reorder();  // No-op

  ASSERT_EQ(4u, scene_objects.NumGeometry());
  EXPECT_EQ(geom0.Get(), &scene_objects.GetGeometry(0).first);
  EXPECT_EQ(matrix1, *scene_objects.GetGeometry(0).second);
  EXPECT_EQ(geom1.Get(), &scene_objects.GetGeometry(1).first);
  EXPECT_EQ(matrix2, *scene_objects.GetGeometry(1).second);
  EXPECT_EQ(geom2.Get(), &scene_objects.GetGeometry(2).first);
  EXPECT_EQ(matrix2, *scene_objects.GetGeometry(2).second);
  EXPECT_EQ(geom3.Get(), &scene_objects.GetGeometry(3).first);
  EXPECT_EQ(nullptr, scene_objects.GetGeometry(3).second);
  EXPECT_EQ(*scene_objects.GetGeometry(1).second,
            *scene_objects.GetGeometry(2).second);
  ASSERT_EQ(3u, scene_objects.NumLights());
  EXPECT_EQ(light0.Get(), &scene_objects.GetLight(0));
  EXPECT_EQ(light1.Get(), &scene_objects.GetLight(1));
  EXPECT_EQ(nullptr, scene_objects.GetEnvironmentalLight());

  // Reorder
  scene_objects.Reorder({{4, 3, 2, 1}}, {{1, 0}});
  EXPECT_EQ(geom0.Get(), &scene_objects.GetGeometry(3).first);
  EXPECT_EQ(matrix1, *scene_objects.GetGeometry(3).second);
  EXPECT_EQ(geom1.Get(), &scene_objects.GetGeometry(2).first);
  EXPECT_EQ(matrix2, *scene_objects.GetGeometry(2).second);
  EXPECT_EQ(geom2.Get(), &scene_objects.GetGeometry(1).first);
  EXPECT_EQ(matrix2, *scene_objects.GetGeometry(1).second);
  EXPECT_EQ(geom3.Get(), &scene_objects.GetGeometry(0).first);
  EXPECT_EQ(nullptr, scene_objects.GetGeometry(0).second);
  EXPECT_EQ(light0.Get(), &scene_objects.GetLight(1));
  EXPECT_EQ(light1.Get(), &scene_objects.GetLight(0));
}