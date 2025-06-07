#include "iris/environmental_lights/internal/distribution_2d.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace environmental_lights {
namespace internal {
namespace {

using ::iris::random::MockRandom;
using ::testing::InSequence;
using ::testing::Return;

TEST(Distribution2D, Sample) {
  std::vector<visual> values = {1.0, 1.0, 3.0, 3.0};
  Distribution2D dist(values, {2, 2});

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.125));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.5));
    Sampler sampler(rng);

    auto [u, v] = dist.Sample(sampler);
    EXPECT_EQ(0.5, u);
    EXPECT_EQ(0.25, v);
  }

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
    Sampler sampler(rng);

    auto [u, v] = dist.Sample(sampler);
    EXPECT_EQ(0.75, u);
    EXPECT_EQ(0.5, v);
  }
}

TEST(Distribution2D, SampleAll) {
  std::vector<visual> values = {1.0, 1.0, 3.0, 3.0};
  Distribution2D dist(values, {2, 2});

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.125));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.5));
    Sampler sampler(rng);

    visual_t pdf;
    size_t offset;
    auto [u, v] = dist.Sample(sampler, &pdf, &offset);
    EXPECT_EQ(0.5, u);
    EXPECT_EQ(0.25, v);
    EXPECT_EQ(0.5, pdf);
    EXPECT_EQ(1u, offset);
  }

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
    Sampler sampler(rng);

    visual_t pdf;
    size_t offset;
    auto [u, v] = dist.Sample(sampler, &pdf, &offset);
    EXPECT_EQ(0.75, u);
    EXPECT_EQ(0.5, v);
    EXPECT_EQ(1.5, pdf);
    EXPECT_EQ(3u, offset);
  }
}

TEST(Distribution2D, Pdf) {
  std::vector<visual> values = {1.0, 1.0, 3.0, 3.0};
  Distribution2D dist(values, {2, 2});

  EXPECT_EQ(0.5, dist.Pdf(0.5, 0.25));
  EXPECT_EQ(1.5, dist.Pdf(0.75, 0.5));
}

}  // namespace
}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris
