#include "iris/sampler.h"

#include <utility>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace {

using ::iris::random::MockRandom;
using ::testing::InSequence;
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

TEST(Sampler, OneVisualSample) {
  MockRandom random;
  EXPECT_CALL(random, NextVisual());
  EXPECT_CALL(random, DiscardGeometric(2));
  {
    Sampler sampler(random);
    sampler.NextLinear1D();
  }
}

TEST(Sampler, OneIndexSample) {
  MockRandom random;
  EXPECT_CALL(random, NextIndex(1u));
  EXPECT_CALL(random, DiscardGeometric(2));
  {
    Sampler sampler(random);
    sampler.NextIndex1D(1u);
  }
}

TEST(Sampler, NextLinear2D) {
  MockRandom random;
  EXPECT_CALL(random, NextGeometric()).Times(2);
  {
    Sampler sampler(random);
    sampler.NextLinear2D();
  }
}

TEST(Sampler, NextPolar) {
  MockRandom random;
  {
    InSequence sequence;
    EXPECT_CALL(random, NextGeometric()).WillOnce(Return(0.25));
    EXPECT_CALL(random, NextGeometric()).WillOnce(Return(0.50));
  }

  {
    Sampler sampler(random);
    auto [radius, theta] = sampler.NextPolar();
    EXPECT_NEAR(-0.5, radius, 0.001);
    EXPECT_NEAR(0.0, theta, 0.001);
  }
}

}  // namespace
}  // namespace iris
