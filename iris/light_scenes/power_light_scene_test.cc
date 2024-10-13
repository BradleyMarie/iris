#include "iris/light_scenes/power_light_scene.h"

#include <limits>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/lights/mock_light.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/scenes/list_scene.h"
#include "iris/testing/light_sample_allocator.h"

namespace iris {
namespace light_scenes {
namespace {

using ::iris::geometry::MockBasicGeometry;
using ::iris::lights::MockLight;
using ::iris::power_matchers::MockPowerMatcher;
using ::iris::random::MockRandom;
using ::iris::testing::GetLightSampleAllocator;
using ::testing::_;
using ::testing::Return;

const static MockPowerMatcher kPowerMatcher;

ReferenceCounted<Geometry> RadiusSqrtTwoGeometry() {
  BoundingBox::Builder builder;
  builder.Add(Point(-1.0, -1.0, 0.0));
  builder.Add(Point(1.0, 1.0, 0.0));

  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::span<const face_t>()));
  EXPECT_CALL(*geometry, ComputeBounds(_)).WillOnce(Return(builder.Build()));

  return geometry;
}

TEST(PowerLightSceneTest, NoLights) {
  random::MockRandom rng;
  SceneObjects scene_objects = SceneObjects::Builder().Build();
  auto light_scene =
      PowerLightScene::Builder::Create()->Build(scene_objects, kPowerMatcher);
  EXPECT_EQ(nullptr, light_scene->Sample(Point(0.0, 0.0, 0.0), rng,
                                         GetLightSampleAllocator()));
}

TEST(PowerLightSceneTest, OneLight) {
  ReferenceCounted<MockLight> light0 = MakeReferenceCounted<MockLight>();
  const Light* light0_ptr = light0.Get();

  MockRandom rng;
  SceneObjects::Builder scene_builder;
  scene_builder.Add(RadiusSqrtTwoGeometry());
  scene_builder.Add(std::move(light0));
  SceneObjects objects = scene_builder.Build();

  std::unique_ptr<LightScene> light_scene =
      PowerLightScene::Builder::Create()->Build(objects, kPowerMatcher);

  const LightSample* light_sample =
      light_scene->Sample(Point(0.0, 0.0, 0.0), rng, GetLightSampleAllocator());
  ASSERT_NE(nullptr, light_sample);

  EXPECT_EQ(light0_ptr, &light_sample->light);
  EXPECT_FALSE(light_sample->pdf);
  ASSERT_EQ(nullptr, light_sample->next);
}

TEST(PowerLightSceneTest, TwoLights) {
  ReferenceCounted<MockLight> light0 = MakeReferenceCounted<MockLight>();
  ReferenceCounted<MockLight> light1 = MakeReferenceCounted<MockLight>();
  const Light* light0_ptr = light0.Get();

  EXPECT_CALL(*light0, Power(_, 2.0)).WillOnce(Return(2.0));
  EXPECT_CALL(*light1, Power(_, 2.0)).WillOnce(Return(1.0));

  MockRandom rng;
  SceneObjects::Builder scene_builder;
  scene_builder.Add(RadiusSqrtTwoGeometry());
  scene_builder.Add(std::move(light0));
  scene_builder.Add(std::move(light1));
  SceneObjects objects = scene_builder.Build();

  std::unique_ptr<LightScene> light_scene =
      PowerLightScene::Builder::Create()->Build(objects, kPowerMatcher);

  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(0.5));

  const LightSample* light_sample =
      light_scene->Sample(Point(0.0, 0.0, 0.0), rng, GetLightSampleAllocator());
  ASSERT_NE(nullptr, light_sample);

  EXPECT_EQ(light0_ptr, &light_sample->light);
  ASSERT_TRUE(light_sample->pdf);
  EXPECT_NEAR(0.666666, *light_sample->pdf, 0.001);
  ASSERT_EQ(nullptr, light_sample->next);
}

}  // namespace
}  // namespace light_scenes
}  // namespace iris