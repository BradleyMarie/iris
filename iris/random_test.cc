#include "iris/random.h"

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace {

using ::iris::random::MockBasicRandom;
using ::testing::Return;

TEST(RandomTest, DiscardGeometric) {
  MockBasicRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(10).WillRepeatedly(Return(1.0));
  rng.DiscardGeometric(10u);
}

}  // namespace
}  // namespace iris
