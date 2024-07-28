#include "iris/light_scenes/power_light_scene.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/lights/mock_light.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/scenes/list_scene.h"
#include "iris/testing/light_sample_allocator.h"

const static iris::power_matchers::MockPowerMatcher kPowerMatcher;

TEST(PowerLightSceneTest, NoLights) {
  iris::random::MockRandom rng;
  auto scene_objects = iris::SceneObjects::Builder().Build();
  auto light_scene =
      iris::light_scenes::PowerLightScene::Builder::Create()->Build(
          scene_objects, kPowerMatcher);
  EXPECT_EQ(nullptr,
            light_scene->Sample(iris::Point(0.0, 0.0, 0.0), rng,
                                iris::testing::GetLightSampleAllocator()));
}

TEST(PowerLightSceneTest, OneLight) {
  auto light0 = iris::MakeReferenceCounted<iris::lights::MockLight>();
  auto* light0_ptr = light0.Get();

  iris::random::MockRandom rng;
  auto light_scene_builder = iris::SceneObjects::Builder();
  light_scene_builder.Add(std::move(light0));
  auto objects = light_scene_builder.Build();
  auto light_scene =
      iris::light_scenes::PowerLightScene::Builder::Create()->Build(
          objects, kPowerMatcher);

  auto light_sample =
      light_scene->Sample(iris::Point(0.0, 0.0, 0.0), rng,
                          iris::testing::GetLightSampleAllocator());
  ASSERT_NE(nullptr, light_sample);

  EXPECT_EQ(light0_ptr, &light_sample->light);
  EXPECT_FALSE(light_sample->pdf);
  ASSERT_EQ(nullptr, light_sample->next);
}

TEST(PowerLightSceneTest, TwoLights) {
  auto light0 = iris::MakeReferenceCounted<iris::lights::MockLight>();
  auto light1 = iris::MakeReferenceCounted<iris::lights::MockLight>();
  auto* light0_ptr = light0.Get();

  EXPECT_CALL(*light0, Power(testing::_)).WillOnce(testing::Return(2.0));
  EXPECT_CALL(*light1, Power(testing::_)).WillOnce(testing::Return(1.0));

  iris::random::MockRandom rng;
  auto light_scene_builder = iris::SceneObjects::Builder();
  light_scene_builder.Add(std::move(light0));
  light_scene_builder.Add(std::move(light1));
  auto objects = light_scene_builder.Build();
  auto light_scene =
      iris::light_scenes::PowerLightScene::Builder::Create()->Build(
          objects, kPowerMatcher);

  EXPECT_CALL(rng, NextVisual()).WillOnce(testing::Return(0.5));

  auto light_sample =
      light_scene->Sample(iris::Point(0.0, 0.0, 0.0), rng,
                          iris::testing::GetLightSampleAllocator());
  ASSERT_NE(nullptr, light_sample);

  EXPECT_EQ(light0_ptr, &light_sample->light);
  ASSERT_TRUE(light_sample->pdf);
  EXPECT_NEAR(0.666666, *light_sample->pdf, 0.001);
  ASSERT_EQ(nullptr, light_sample->next);
}