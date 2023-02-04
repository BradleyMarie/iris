#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/image_samplers/internal/mock_low_discrepancy_sequence.h"
#include "iris/random/mock_random.h"

static iris::random::MockRandom g_rng;

TEST(LowDiscrepancyImageSamplerTest, NextSampleDesiredReached) {
  auto sequence = std::make_unique<
      iris::image_samplers::internal::MockLowDiscrepancySequence>();

  iris::image_samplers::internal::LowDiscrepancyImageSampler sampler(
      std::move(sequence), 0);

  sampler.StartPixel({0, 0}, {0, 0});

  EXPECT_FALSE(sampler.NextSample(false, g_rng));
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleNone) {
  auto sequence = std::make_unique<
      iris::image_samplers::internal::MockLowDiscrepancySequence>();

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(false));

  iris::image_samplers::internal::LowDiscrepancyImageSampler sampler(
      std::move(sequence), 1);
  sampler.StartPixel({0, 0}, {0, 0});

  EXPECT_FALSE(sampler.NextSample(false, g_rng));
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleNoLens) {
  auto sequence = std::make_unique<
      iris::image_samplers::internal::MockLowDiscrepancySequence>();
  auto sequence_ptr = sequence.get();

  {
    testing::InSequence s;
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.25));
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.75));
  }

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(true));

  EXPECT_CALL(*sequence, SampleWeight(1)).WillOnce(testing::Return(1.0));

  iris::image_samplers::internal::LowDiscrepancyImageSampler sampler(
      std::move(sequence), 1);
  sampler.StartPixel({0, 0}, {0, 0});

  auto sample = sampler.NextSample(false, g_rng);
  ASSERT_TRUE(sample);
  EXPECT_EQ(0.25, sample->image_uv[0]);
  EXPECT_EQ(0.75, sample->image_uv[1]);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(1.0, sample->weight);
  EXPECT_EQ(sequence_ptr, &sample->rng);
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleWithLens) {
  auto sequence = std::make_unique<
      iris::image_samplers::internal::MockLowDiscrepancySequence>();
  auto sequence_ptr = sequence.get();

  {
    testing::InSequence s;
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.25));
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.75));
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.125));
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.375));
  }

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(true));

  EXPECT_CALL(*sequence, SampleWeight(1)).WillOnce(testing::Return(1.0));

  iris::image_samplers::internal::LowDiscrepancyImageSampler sampler(
      std::move(sequence), 1);
  sampler.StartPixel({0, 0}, {0, 0});

  auto sample = sampler.NextSample(true, g_rng);
  ASSERT_TRUE(sample);
  EXPECT_EQ(0.25, sample->image_uv[0]);
  EXPECT_EQ(0.75, sample->image_uv[1]);
  ASSERT_TRUE(sample->lens_uv);
  EXPECT_EQ(0.125, (*sample->lens_uv)[0]);
  EXPECT_EQ(0.375, (*sample->lens_uv)[1]);
  EXPECT_EQ(1.0, sample->weight);
  EXPECT_EQ(sequence_ptr, &sample->rng);
}

TEST(LowDiscrepancyImageSamplerTest, TwoSamples) {
  auto sequence = std::make_unique<
      iris::image_samplers::internal::MockLowDiscrepancySequence>();

  {
    testing::InSequence s;
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.25));
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.75));
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.125));
    EXPECT_CALL(*sequence, Next()).WillOnce(testing::Return(0.375));
  }

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 1))
      .WillOnce(testing::Return(true));

  EXPECT_CALL(*sequence, SampleWeight(2)).WillRepeatedly(testing::Return(0.5));

  iris::image_samplers::internal::LowDiscrepancyImageSampler sampler(
      std::move(sequence), 2);
  sampler.StartPixel({0, 0}, {0, 0});

  EXPECT_TRUE(sampler.NextSample(false, g_rng));
  EXPECT_TRUE(sampler.NextSample(false, g_rng));
  EXPECT_FALSE(sampler.NextSample(false, g_rng));
}

TEST(LowDiscrepancyImageSamplerTest, Replicate) {
  auto sequence = std::make_unique<
      iris::image_samplers::internal::MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Duplicate()).WillOnce(testing::Invoke([]() {
    return std::make_unique<
        iris::image_samplers::internal::MockLowDiscrepancySequence>();
  }));
  iris::image_samplers::internal::LowDiscrepancyImageSampler sampler(
      std::move(sequence), 2);
  EXPECT_TRUE(sampler.Replicate());
}