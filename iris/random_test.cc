#include "iris/random.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

TEST(RandomTest, DiscardIndex) {
  iris::random::MockBasicRandom rng;
  EXPECT_CALL(rng, NextIndex(128u))
      .Times(10)
      .WillRepeatedly(testing::Return(1));
  rng.DiscardIndex(10u);
}

TEST(RandomTest, DiscardGeometric) {
  iris::random::MockBasicRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(10)
      .WillRepeatedly(testing::Return(1.0));
  rng.DiscardGeometric(10u);
}

TEST(RandomTest, DiscardVisual) {
  iris::random::MockBasicRandom rng;
  EXPECT_CALL(rng, NextVisual()).Times(10).WillRepeatedly(testing::Return(1.0));
  rng.DiscardVisual(10u);
}
