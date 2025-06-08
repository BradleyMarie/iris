#include "iris/light_scenes/all_light_scene.h"

#include <limits>
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/light.h"
#include "iris/light_sample.h"
#include "iris/light_scene.h"
#include "iris/lights/mock_light.h"
#include "iris/point.h"
#include "iris/power_matchers/mock_power_matcher.h"
#include "iris/random/mock_random.h"
#include "iris/reference_counted.h"
#include "iris/scene_objects.h"
#include "iris/scenes/list_scene.h"
#include "iris/testing/light_sample_allocator.h"

namespace iris {
namespace light_scenes {
namespace {

using ::iris::lights::MockLight;
using ::iris::power_matchers::MockPowerMatcher;
using ::iris::random::MockRandom;
using ::iris::testing::GetLightSampleAllocator;

const static MockPowerMatcher kPowerMatcher;

TEST(AllLightSceneTest, NoLights) {
  MockRandom rng;
  SceneObjects scene_objects = SceneObjects::Builder().Build();
  std::unique_ptr<LightScene> light_scene =
      MakeAllLightSceneBuilder()->Build(scene_objects, kPowerMatcher);
  EXPECT_EQ(nullptr, light_scene->Sample(Point(0.0, 0.0, 0.0), rng,
                                         GetLightSampleAllocator()));
}

TEST(AllLightSceneTest, TwoLights) {
  ReferenceCounted<MockLight> light0 = MakeReferenceCounted<MockLight>();
  const Light* light0_ptr = light0.Get();

  ReferenceCounted<MockLight> light1 = MakeReferenceCounted<MockLight>();
  const Light* light1_ptr = light1.Get();

  MockRandom rng;

  SceneObjects::Builder scene_builder;
  scene_builder.Add(std::move(light0));
  scene_builder.Add(std::move(light1));
  SceneObjects objects = scene_builder.Build();

  std::unique_ptr<LightScene> light_scene =
      MakeAllLightSceneBuilder()->Build(objects, kPowerMatcher);

  const LightSample* light_samples =
      light_scene->Sample(Point(0.0, 0.0, 0.0), rng, GetLightSampleAllocator());
  ASSERT_NE(nullptr, light_samples);

  const LightSample* light1_sample = light_samples;
  EXPECT_EQ(light1_ptr, &light1_sample->light);
  EXPECT_FALSE(light1_sample->pdf);
  ASSERT_NE(nullptr, light1_sample->next);

  const LightSample* light0_sample = light1_sample->next;
  EXPECT_EQ(light0_ptr, &light0_sample->light);
  EXPECT_FALSE(light0_sample->pdf);
  EXPECT_EQ(nullptr, light0_sample->next);
}

}  // namespace
}  // namespace light_scenes
}  // namespace iris
