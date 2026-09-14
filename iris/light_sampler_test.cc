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
#include "iris/testing/sampler.h"

namespace iris {
namespace {

using ::iris::light_scenes::MockLightScene;
using ::iris::lights::MockLight;
using ::iris::testing::MakeSampler;
using ::testing::_;

TEST(LightSamplerTest, Sample) {
  std::unique_ptr<Light> light = std::make_unique<MockLight>();
  Sampler samp = MakeSampler({}, {});
  internal::Arena arena;
  LightSampleAllocator alloc(arena);

  MockLightScene scene;
  EXPECT_CALL(scene, Sample(Point(1.0, 1.0, 1.0), _, _))
      .WillOnce([&](const Point& hit_point, Sampler& sampler,
                    LightSampleAllocator& allocator) {
        EXPECT_EQ(&samp, &sampler);
        EXPECT_EQ(&alloc, &allocator);
        return &allocator.Allocate(*light, static_cast<visual_t>(2.0));
      });

  LightSampler sampler(scene, alloc);

  const LightSample* sample = sampler.Sample(Point(1.0, 1.0, 1.0), samp);
  ASSERT_NE(nullptr, sample);
  EXPECT_EQ(light.get(), &sample->light);
  EXPECT_EQ(2.0, sample->pdf);
  EXPECT_EQ(nullptr, sample->next);
}

}  // namespace
}  // namespace iris
