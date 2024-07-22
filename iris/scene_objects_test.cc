#include "iris/scene_objects.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/lights/mock_light.h"

iris::ReferenceCounted<iris::Geometry> MakeZeroBoundsGeometry() {
  auto result = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*result, ComputeBounds(nullptr))
      .WillRepeatedly(
          testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0))));
  return result;
}

iris::ReferenceCounted<iris::Geometry> MakeGeometry(bool emissive) {
  static const std::vector<iris::face_t> faces = {1};
  auto result = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*result, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(emissive));
  EXPECT_CALL(*result, GetFaces()).WillRepeatedly(testing::Return(faces));
  EXPECT_CALL(*result, ComputeBounds(testing::_))
      .WillRepeatedly(testing::Return(iris::BoundingBox(
          iris::Point(0.0, 0.0, 0.0), iris::Point(0.0, 1.0, 2.0))));
  return result;
}

std::set<const iris::Light*> GetLights(const iris::SceneObjects& objects) {
  std::set<const iris::Light*> result;
  for (size_t i = 0; i < objects.NumLights(); i++) {
    result.emplace(&objects.GetLight(i));
  }
  return result;
}

TEST(SceneObjects, Build) {
  std::set<iris::ReferenceCounted<iris::Geometry>> geometry;
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(true));
  geometry.insert(MakeZeroBoundsGeometry());

  auto geometry_iter = geometry.begin();
  iris::ReferenceCounted<iris::Geometry> geom0 = *geometry_iter++;
  iris::ReferenceCounted<iris::Geometry> geom1 = *geometry_iter++;
  iris::ReferenceCounted<iris::Geometry> geom2 = *geometry_iter++;
  iris::ReferenceCounted<iris::Geometry> geom3 = *geometry_iter++;
  iris::ReferenceCounted<iris::Geometry> geom4 = *geometry_iter++;

  std::set<iris::ReferenceCounted<iris::Light>> lights;
  lights.insert(iris::MakeReferenceCounted<iris::lights::MockLight>());
  lights.insert(iris::MakeReferenceCounted<iris::lights::MockLight>());

  auto light_iter = lights.begin();
  iris::ReferenceCounted<iris::Light> light0 = *light_iter++;
  iris::ReferenceCounted<iris::Light> light1 = *light_iter++;

  auto environmental_light = iris::MakeReferenceCounted<
      iris::environmental_lights::MockEnvironmentalLight>();
  auto matrix0 = iris::Matrix::Identity();
  auto matrix1 = iris::Matrix::Translation(1.0, 2.0, 3.0).value();
  auto matrix2 = iris::Matrix::Scalar(1.0, 2.0, 3.0).value();

  iris::SceneObjects::Builder builder;

  builder.Add(iris::ReferenceCounted<iris::lights::MockLight>());
  builder.Add(iris::ReferenceCounted<iris::Geometry>(), matrix0);

  builder.Add(light0);
  builder.Add(light0);
  builder.Add(light1);
  builder.Add(light1);

  builder.Add(geom0, matrix1);
  builder.Add(geom0, matrix1);
  builder.Add(geom1, matrix2);
  builder.Add(geom1, matrix2);
  builder.Add(geom2, matrix2);
  builder.Add(geom2, matrix2);
  builder.Add(geom3, matrix0);
  builder.Add(geom3, matrix0);

  builder.Add(geom4);
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
  auto built_lights = GetLights(scene_objects);
  ASSERT_EQ(4u, built_lights.size());
  built_lights.erase(light0.Get());
  built_lights.erase(light1.Get());
  ASSERT_EQ(2u, built_lights.size());
  EXPECT_EQ(environmental_light.Get(), scene_objects.GetEnvironmentalLight());

  // Reuse builder
  geometry.clear();
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(true));

  geometry_iter = geometry.begin();
  geom0 = *geometry_iter++;
  geom1 = *geometry_iter++;
  geom2 = *geometry_iter++;
  geom3 = *geometry_iter++;

  lights.clear();
  lights.insert(iris::MakeReferenceCounted<iris::lights::MockLight>());
  lights.insert(iris::MakeReferenceCounted<iris::lights::MockLight>());

  light_iter = lights.begin();
  light0 = *light_iter++;
  light1 = *light_iter++;

  builder.Add(light0);
  builder.Add(light0);
  builder.Add(light1);
  builder.Add(light1);

  builder.Add(geom0, matrix1);
  builder.Add(geom0, matrix1);
  builder.Add(geom1, matrix2);
  builder.Add(geom1, matrix2);
  builder.Add(geom2, matrix2);
  builder.Add(geom2, matrix2);
  builder.Add(geom3, matrix0);
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
  built_lights = GetLights(scene_objects);
  ASSERT_EQ(3u, built_lights.size());
  built_lights.erase(light0.Get());
  built_lights.erase(light1.Get());
  ASSERT_EQ(1u, built_lights.size());
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