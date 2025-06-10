#include "iris/light_sampler.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/internal/arena.h"
#include "iris/light_sample.h"
#include "iris/light_sample_allocator.h"
#include "iris/light_scenes/mock_light_scene.h"
#include "iris/lights/mock_light.h"
#include "iris/point.h"
#include "iris/random.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace {

using ::iris::light_scenes::MockLightScene;
using ::iris::lights::MockLight;
using ::iris::random::MockRandom;
using ::testing::_;
using ::testing::Invoke;

TEST(LightSamplerTest, Sample) {
  std::unique_ptr<Light> light = std::make_unique<MockLight>();
  std::unique_ptr<Random> random = std::make_unique<MockRandom>();
  internal::Arena arena;
  LightSampleAllocator allocator(arena);

  MockLightScene scene;
  EXPECT_CALL(scene, Sample(Point(1.0, 1.0, 1.0), _, _))
      .WillOnce(Invoke([&](const Point& hit_point, Random& rng,
                           LightSampleAllocator& allocator) {
        EXPECT_EQ(random.get(), &rng);
        EXPECT_EQ(&allocator, &allocator);
        return &allocator.Allocate(*light, static_cast<visual_t>(2.0));
      }));

  LightSampler sampler(scene, *random, allocator);

  const LightSample* sample = sampler.Sample(Point(1.0, 1.0, 1.0));
  ASSERT_NE(nullptr, sample);
  EXPECT_EQ(light.get(), &sample->light);
  EXPECT_EQ(2.0, sample->pdf);
  EXPECT_EQ(nullptr, sample->next);
}

}  // namespace
}  // namespace iris
