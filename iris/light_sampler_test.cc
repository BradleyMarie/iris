#include "iris/light_sampler.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/internal/arena.h"
#include "iris/lights/mock_light.h"
#include "iris/random/mock_random.h"

static const std::unique_ptr<iris::Light> g_light =
    std::make_unique<iris::lights::MockLight>();
static const std::unique_ptr<iris::Random> g_random =
    std::make_unique<iris::random::MockRandom>();

class TestLightScene final : public iris::LightScene {
 public:
  TestLightScene(const iris::Point& hit_point, iris::Random& rng,
                 iris::LightSampleAllocator& allocator)
      : expected_hit_point_(hit_point),
        expected_rng_(rng),
        allocator_(allocator) {}

  iris::LightSample* Sample(
      const iris ::Point& hit_point, iris::Random& rng,
      iris ::LightSampleAllocator& allocator) const override {
    EXPECT_EQ(expected_hit_point_, hit_point);
    EXPECT_EQ(&expected_rng_, &rng);
    EXPECT_EQ(&allocator_, &allocator);
    return &allocator.Allocate(*g_light, 2.0);
  }

  iris::Point expected_hit_point_;
  iris::Random& expected_rng_;
  iris::LightSampleAllocator& allocator_;
};

TEST(LightSamplerTest, Sample) {
  iris::internal::Arena arena;
  iris::LightSampleAllocator allocator(arena);

  TestLightScene scene(iris::Point(1.0, 1.0, 1.0), *g_random, allocator);
  iris::LightSampler sampler(scene, *g_random, allocator);

  auto sample = sampler.Sample(iris::Point(1.0, 1.0, 1.0));
  ASSERT_NE(nullptr, sample);
  EXPECT_EQ(g_light.get(), &sample->light);
  EXPECT_EQ(2.0, sample->pdf);
  EXPECT_EQ(nullptr, sample->next);
}