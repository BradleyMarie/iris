#include "iris/sampler.h"

#include <utility>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace {

using ::iris::random::MockRandom;
using ::testing::Return;

TEST(Sampler, MoveConstruct) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));
  Sampler sampler(random);
  {
    Sampler sampler2(std::move(sampler));
  }
}

TEST(Sampler, NoSamples) {
  MockRandom random;
  EXPECT_CALL(random, DiscardGeometric(2));
  {
    Sampler sampler(random);
  }
}

TEST(Sampler, OneSample) {
  MockRandom random;
  EXPECT_CALL(random, NextGeometric());
  EXPECT_CALL(random, DiscardGeometric(1));
  {
    Sampler sampler(random);
    sampler.Next();
  }
}

TEST(Sampler, TwoSamples) {
  MockRandom random;
  EXPECT_CALL(random, NextGeometric()).Times(2);
  {
    Sampler sampler(random);
    sampler.Next();
    sampler.Next();
  }
}

TEST(Sampler, TwoSamplesWithIndex) {
  MockRandom random;
  EXPECT_CALL(random, NextGeometric()).Times(2);
  {
    Sampler sampler(random);
    sampler.NextIndex(2);
    sampler.Next();
    sampler.Next();
  }
}

TEST(Sampler, NextIndex) {
  MockRandom random;
  EXPECT_CALL(random, NextGeometric()).WillOnce(Return(0.3));
  EXPECT_CALL(random, DiscardGeometric(1));
  {
    Sampler sampler(random);
    EXPECT_EQ(1u, sampler.NextIndex(4));
    EXPECT_NEAR(0.2, sampler.Next(), 0.001);
  }
}

}  // namespace
}  // namespace iris
