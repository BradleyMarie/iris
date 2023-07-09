#include "iris/environmental_lights/internal/distribution_2d.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

TEST(Distribution2D, Sample) {
  std::vector<iris::visual> values = {1.0, 1.0, 3.0, 3.0};
  iris::environmental_lights::internal::Distribution2D dist(values, {2, 2});

  {
    testing::InSequence sequence;

    iris::random::MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.125));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.5));
    iris::Sampler sampler(rng);

    auto [u, v] = dist.Sample(sampler);
    EXPECT_EQ(0.5, u);
    EXPECT_EQ(0.25, v);
  }

  {
    testing::InSequence sequence;

    iris::random::MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
    iris::Sampler sampler(rng);

    auto [u, v] = dist.Sample(sampler);
    EXPECT_EQ(0.75, u);
    EXPECT_EQ(0.5, v);
  }
}

TEST(Distribution2D, SampleAll) {
  std::vector<iris::visual> values = {1.0, 1.0, 3.0, 3.0};
  iris::environmental_lights::internal::Distribution2D dist(values, {2, 2});

  {
    testing::InSequence sequence;

    iris::random::MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.125));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.5));
    iris::Sampler sampler(rng);

    iris::visual_t pdf;
    size_t offset;
    auto [u, v] = dist.Sample(sampler, &pdf, &offset);
    EXPECT_EQ(0.5, u);
    EXPECT_EQ(0.25, v);
    EXPECT_EQ(0.5, pdf);
    EXPECT_EQ(1u, offset);
  }

  {
    testing::InSequence sequence;

    iris::random::MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
    iris::Sampler sampler(rng);

    iris::visual_t pdf;
    size_t offset;
    auto [u, v] = dist.Sample(sampler, &pdf, &offset);
    EXPECT_EQ(0.75, u);
    EXPECT_EQ(0.5, v);
    EXPECT_EQ(1.5, pdf);
    EXPECT_EQ(3u, offset);
  }
}

TEST(Distribution2D, Pdf) {
  std::vector<iris::visual> values = {1.0, 1.0, 3.0, 3.0};
  iris::environmental_lights::internal::Distribution2D dist(values, {2, 2});

  EXPECT_EQ(0.5, dist.Pdf(0.5, 0.25));
  EXPECT_EQ(1.5, dist.Pdf(0.75, 0.5));
}