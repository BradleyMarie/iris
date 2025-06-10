#include "iris/random.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace {

using ::iris::random::MockBasicRandom;
using ::testing::Return;

TEST(RandomTest, DiscardIndex) {
  MockBasicRandom rng;
  EXPECT_CALL(rng, NextIndex(128u)).Times(10).WillRepeatedly(Return(1));
  rng.DiscardIndex(10u);
}

TEST(RandomTest, DiscardGeometric) {
  MockBasicRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(10).WillRepeatedly(Return(1.0));
  rng.DiscardGeometric(10u);
}

TEST(RandomTest, DiscardVisual) {
  MockBasicRandom rng;
  EXPECT_CALL(rng, NextVisual()).Times(10).WillRepeatedly(Return(1.0));
  rng.DiscardVisual(10u);
}

}  // namespace
}  // namespace iris
