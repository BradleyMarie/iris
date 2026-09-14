#include "iris/integrators/internal/russian_roulette.h"

#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/testing/sampler.h"

namespace iris {
namespace integrators {
namespace internal {
namespace {

using ::iris::testing::MakeSampler;

TEST(RussianRoulette, AboveThreshold) {
  Sampler sampler = MakeSampler({0.0, 0.0}, {});

  RussianRoulette roulette(0.95, 100.0);
  EXPECT_NEAR(0.95, roulette.Evaluate(sampler, 1.0).value(), 0.001);
  EXPECT_EQ(1.00, roulette.Evaluate(sampler, 100.0));
}

TEST(RussianRoulette, AboveMaximumSuccess) {
  Sampler sampler = MakeSampler({0.0}, {});

  RussianRoulette roulette(0.75, 1.0);
  std::optional<visual_t> result = roulette.Evaluate(sampler, 0.99);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(0.75, result.value(), 0.0001);
}

TEST(RussianRoulette, AboveMaximumFails) {
  Sampler sampler = MakeSampler({1.0}, {});

  RussianRoulette roulette(0.75, 1.0);
  std::optional<visual_t> result = roulette.Evaluate(sampler, 0.99);
  ASSERT_FALSE(result.has_value());
}

TEST(RussianRoulette, BelowMaximumSuccess) {
  Sampler sampler = MakeSampler({0.0}, {});

  RussianRoulette roulette(0.75, 1.0);
  std::optional<visual_t> result = roulette.Evaluate(sampler, 0.5);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(0.5, result.value(), 0.0001);
}

TEST(RussianRoulette, BelowMaximumFails) {
  Sampler sampler = MakeSampler({1.0}, {});

  RussianRoulette roulette(0.75, 1.0);
  std::optional<visual_t> result = roulette.Evaluate(sampler, 0.5);
  ASSERT_FALSE(result.has_value());
}

}  // namespace
}  // namespace internal
}  // namespace integrators
}  // namespace iris
