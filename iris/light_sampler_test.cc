#include "iris/light_sampler.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/internal/arena.h"
#include "iris/light_scenes/mock_light_scene.h"
#include "iris/lights/mock_light.h"
#include "iris/random/mock_random.h"

TEST(LightSamplerTest, Sample) {
  std::unique_ptr<iris::Light> light =
      std::make_unique<iris::lights::MockLight>();
  std::unique_ptr<iris::Random> random =
      std::make_unique<iris::random::MockRandom>();
  iris::internal::Arena arena;
  iris::LightSampleAllocator allocator(arena);

  iris::light_scenes::MockLightScene scene;
  EXPECT_CALL(scene, Sample(iris::Point(1.0, 1.0, 1.0), testing::_, testing::_))
      .WillOnce(testing::Invoke([&](const iris ::Point& hit_point,
                                    iris::Random& rng,
                                    iris ::LightSampleAllocator& allocator) {
        EXPECT_EQ(random.get(), &rng);
        EXPECT_EQ(&allocator, &allocator);
        return &allocator.Allocate(*light, static_cast<iris::visual_t>(2.0));
      }));

  iris::LightSampler sampler(scene, *random, allocator);

  auto sample = sampler.Sample(iris::Point(1.0, 1.0, 1.0));
  ASSERT_NE(nullptr, sample);
  EXPECT_EQ(light.get(), &sample->light);
  EXPECT_EQ(2.0, sample->pdf);
  EXPECT_EQ(nullptr, sample->next);
}