#include "iris/integrators/internal/russian_roulette.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace integrators {
namespace internal {
namespace {

using ::iris::random::MockRandom;
using ::testing::Return;

TEST(RussianRoulette, AboveThreshold) {
  MockRandom rng;
  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(0.0));
  EXPECT_CALL(rng, DiscardVisual(1));

  RussianRoulette roulette(0.95, 100.0);
  EXPECT_NEAR(0.95, roulette.Evaluate(rng, 1.0).value(), 0.001);
  EXPECT_EQ(1.00, roulette.Evaluate(rng, 100.0));
}

TEST(RussianRoulette, AboveMaximumSuccess) {
  MockRandom rng;
  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(0.0));

  RussianRoulette roulette(0.75, 1.0);
  auto result = roulette.Evaluate(rng, 0.99);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(0.75, result.value(), 0.0001);
}

TEST(RussianRoulette, AboveMaximumFails) {
  MockRandom rng;
  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(1.0));

  RussianRoulette roulette(0.75, 1.0);
  auto result = roulette.Evaluate(rng, 0.99);
  ASSERT_FALSE(result.has_value());
}

TEST(RussianRoulette, BelowMaximumSuccess) {
  MockRandom rng;
  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(0.0));

  RussianRoulette roulette(0.75, 1.0);
  auto result = roulette.Evaluate(rng, 0.5);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(0.5, result.value(), 0.0001);
}

TEST(RussianRoulette, BelowMaximumFails) {
  MockRandom rng;
  EXPECT_CALL(rng, NextVisual()).WillOnce(Return(1.0));

  RussianRoulette roulette(0.75, 1.0);
  auto result = roulette.Evaluate(rng, 0.5);
  ASSERT_FALSE(result.has_value());
}

}  // namespace
}  // namespace internal
}  // namespace integrators
}  // namespace iris