#include "iris/light_scenes/one_light_scene.h"

#include <limits>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/lights/mock_light.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/scenes/list_scene.h"
#include "iris/testing/light_sample_allocator.h"

namespace iris {
namespace light_scenes {
namespace {

using ::iris::lights::MockLight;
using ::iris::power_matchers::MockPowerMatcher;
using ::iris::random::MockRandom;
using ::iris::testing::GetLightSampleAllocator;
using ::testing::Return;

const static MockPowerMatcher kPowerMatcher;

TEST(OneLightSceneTest, NoLights) {
  MockRandom rng;
  SceneObjects scene_objects = SceneObjects::Builder().Build();
  std::unique_ptr<LightScene> light_scene =
      OneLightScene::Builder::Create()->Build(scene_objects, kPowerMatcher);
  EXPECT_EQ(nullptr, light_scene->Sample(Point(0.0, 0.0, 0.0), rng,
                                         GetLightSampleAllocator()));
}

TEST(OneLightSceneTest, OneLight) {
  ReferenceCounted<MockLight> light0 = MakeReferenceCounted<MockLight>();
  const Light* light0_ptr = light0.Get();

  MockRandom rng;
  SceneObjects::Builder scene_builder;
  scene_builder.Add(std::move(light0));
  SceneObjects objects = scene_builder.Build();

  std::unique_ptr<LightScene> light_scene =
      OneLightScene::Builder::Create()->Build(objects, kPowerMatcher);

  const LightSample* light_sample =
      light_scene->Sample(Point(0.0, 0.0, 0.0), rng, GetLightSampleAllocator());
  ASSERT_NE(nullptr, light_sample);

  EXPECT_EQ(light0_ptr, &light_sample->light);
  EXPECT_FALSE(light_sample->pdf);
  ASSERT_EQ(nullptr, light_sample->next);
}

TEST(OneLightSceneTest, TwoLights) {
  ReferenceCounted<MockLight> light0 = MakeReferenceCounted<MockLight>();
  ReferenceCounted<MockLight> light1 = MakeReferenceCounted<MockLight>();
  const Light* light1_ptr = light1.Get();

  MockRandom rng;
  SceneObjects::Builder scene_builder;
  scene_builder.Add(std::move(light0));
  scene_builder.Add(std::move(light1));
  SceneObjects objects = scene_builder.Build();

  std::unique_ptr<LightScene> light_scene =
      OneLightScene::Builder::Create()->Build(objects, kPowerMatcher);

  EXPECT_CALL(rng, NextIndex(2)).WillOnce(Return(1));

  const LightSample* light_sample =
      light_scene->Sample(Point(0.0, 0.0, 0.0), rng, GetLightSampleAllocator());
  ASSERT_NE(nullptr, light_sample);

  EXPECT_EQ(light1_ptr, &light_sample->light);
  EXPECT_EQ(0.5, light_sample->pdf);
  ASSERT_EQ(nullptr, light_sample->next);
}

}  // namespace
}  // namespace light_scenes
}  // namespace iris