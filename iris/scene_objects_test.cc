#include "iris/scene_objects.h"

#include <set>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/geometry.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/integer.h"
#include "iris/light.h"
#include "iris/lights/mock_light.h"
#include "iris/matrix.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/vector.h"

namespace iris {
namespace {

using ::iris::emissive_materials::MockEmissiveMaterial;
using ::iris::environmental_lights::MockEnvironmentalLight;
using ::iris::geometry::MockGeometry;
using ::iris::lights::MockLight;
using ::testing::_;
using ::testing::Return;

const static BoundingBox kBounds(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0));

ReferenceCounted<Geometry> MakeZeroBoundsGeometry() {
  ReferenceCounted<MockGeometry> result = MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*result, ComputeBounds(nullptr))
      .WillRepeatedly(Return(BoundingBox(Point(0.0, 0.0, 0.0))));
  return result;
}

ReferenceCounted<Geometry> MakeGeometry(bool emissive) {
  static const std::vector<face_t> faces = {1};
  static const MockEmissiveMaterial emissive_material;
  ReferenceCounted<MockGeometry> result = MakeReferenceCounted<MockGeometry>();
  if (emissive) {
    EXPECT_CALL(*result, GetEmissiveMaterial(_))
        .WillRepeatedly(Return(&emissive_material));
  } else {
    EXPECT_CALL(*result, GetEmissiveMaterial(_))
        .WillRepeatedly(Return(nullptr));
  }
  EXPECT_CALL(*result, GetFaces()).WillRepeatedly(Return(faces));
  EXPECT_CALL(*result, ComputeBounds(_)).WillRepeatedly(Return(kBounds));
  return result;
}

std::set<const Light*> GetLights(const SceneObjects& objects) {
  std::set<const Light*> result;
  for (size_t i = 0; i < objects.NumLights(); i++) {
    result.emplace(&objects.GetLight(i));
  }
  return result;
}

TEST(SceneObjects, Build) {
  std::set<ReferenceCounted<Geometry>> geometry;
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(false));
  geometry.insert(MakeGeometry(true));
  geometry.insert(MakeZeroBoundsGeometry());

  auto geometry_iter = geometry.begin();
  ReferenceCounted<Geometry> geom0 = *geometry_iter++;
  ReferenceCounted<Geometry> geom1 = *geometry_iter++;
  ReferenceCounted<Geometry> geom2 = *geometry_iter++;
  ReferenceCounted<Geometry> geom3 = *geometry_iter++;
  ReferenceCounted<Geometry> geom4 = *geometry_iter++;

  std::set<ReferenceCounted<Light>> lights;
  lights.insert(MakeReferenceCounted<MockLight>());
  lights.insert(MakeReferenceCounted<MockLight>());

  auto light_iter = lights.begin();
  ReferenceCounted<Light> light0 = *light_iter++;
  ReferenceCounted<Light> light1 = *light_iter++;

  ReferenceCounted<MockEnvironmentalLight> environmental_light =
      MakeReferenceCounted<MockEnvironmentalLight>();
  Matrix matrix0 = Matrix::Identity();
  Matrix matrix1 = Matrix::Translation(1.0, 2.0, 3.0).value();
  Matrix matrix2 = Matrix::Scalar(1.0, 2.0, 3.0).value();

  SceneObjects::Builder builder;

  builder.Add(ReferenceCounted<MockLight>());
  builder.Add(ReferenceCounted<Geometry>(), matrix0);

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

  SceneObjects scene_objects = builder.Build();
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
  std::set<const Light*> built_lights = GetLights(scene_objects);
  ASSERT_EQ(4u, built_lights.size());
  built_lights.erase(light0.Get());
  built_lights.erase(light1.Get());
  ASSERT_EQ(2u, built_lights.size());
  EXPECT_EQ(environmental_light.Get(), scene_objects.GetEnvironmentalLight());
  EXPECT_EQ(kBounds, scene_objects.GetBounds());

  // Reuse builder for empty scene
  scene_objects = builder.Build();
  EXPECT_TRUE(scene_objects.GetBounds().Empty());

  // Reuse builder for populated scene
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
  lights.insert(MakeReferenceCounted<MockLight>());
  lights.insert(MakeReferenceCounted<MockLight>());

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
  EXPECT_EQ(kBounds, scene_objects.GetBounds());

  const Light& light0_old = scene_objects.GetLight(0);
  const Light& light1_old = scene_objects.GetLight(1);

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
  EXPECT_EQ(&light0_old, &scene_objects.GetLight(1));
  EXPECT_EQ(&light1_old, &scene_objects.GetLight(0));
}

}  // namespace
}  // namespace iris
