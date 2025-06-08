#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/image_samplers/internal/mock_low_discrepancy_sequence.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace image_samplers {
namespace internal {
namespace {

TEST(LowDiscrepancyImageSamplerTest, NextSampleDesiredReached) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();

  LowDiscrepancyImageSampler sampler(std::move(sequence), 0);

  sampler.StartPixel({0, 0}, {0, 0});

  random::MockRandom rng;
  EXPECT_FALSE(sampler.NextSample(false, rng));
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleNone) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(false));

  LowDiscrepancyImageSampler sampler(std::move(sequence), 1);
  sampler.StartPixel({0, 0}, {0, 0});

  random::MockRandom rng;
  EXPECT_FALSE(sampler.NextSample(false, rng));
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleNoLens) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  auto sequence_ptr = sequence.get();

  {
    testing::InSequence s;
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.25)));
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.75)));
  }

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(true));

  EXPECT_CALL(*sequence, SampleWeight(1))
      .WillOnce(testing::Return(static_cast<visual_t>(1.0)));

  LowDiscrepancyImageSampler sampler(std::move(sequence), 1);
  sampler.StartPixel({1, 1}, {0, 0});

  random::MockRandom rng;
  auto sample = sampler.NextSample(false, rng);
  ASSERT_TRUE(sample);
  EXPECT_EQ(0.25, sample->image_uv[0]);
  EXPECT_EQ(0.75, sample->image_uv[1]);
  EXPECT_NEAR(sample->image_uv[1] + 1.0, sample->image_uv_dxdy[1], 0.01);
  EXPECT_NEAR(sample->image_uv[0] + 1.0, sample->image_uv_dxdy[0], 0.01);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(1.0, sample->weight);
  EXPECT_EQ(sequence_ptr, &sample->rng);
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleWithLens) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  auto sequence_ptr = sequence.get();

  {
    testing::InSequence s;
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.25)));
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.75)));
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.125)));
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.375)));
  }

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(true));

  EXPECT_CALL(*sequence, SampleWeight(1))
      .WillOnce(testing::Return(static_cast<visual_t>(1.0)));

  LowDiscrepancyImageSampler sampler(std::move(sequence), 1);
  sampler.StartPixel({1, 1}, {0, 0});

  random::MockRandom rng;
  auto sample = sampler.NextSample(true, rng);
  ASSERT_TRUE(sample);
  EXPECT_EQ(0.25, sample->image_uv[0]);
  EXPECT_EQ(0.75, sample->image_uv[1]);
  EXPECT_NEAR(sample->image_uv[0] + 1.0, sample->image_uv_dxdy[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 1.0, sample->image_uv_dxdy[1], 0.01);
  ASSERT_TRUE(sample->lens_uv);
  EXPECT_EQ(0.125, (*sample->lens_uv)[0]);
  EXPECT_EQ(0.375, (*sample->lens_uv)[1]);
  EXPECT_EQ(1.0, sample->weight);
  EXPECT_EQ(sequence_ptr, &sample->rng);
}

TEST(LowDiscrepancyImageSamplerTest, TwoSamples) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();

  {
    testing::InSequence s;
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.25)));
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.75)));
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.125)));
    EXPECT_CALL(*sequence, Next())
        .WillOnce(testing::Return(static_cast<visual_t>(0.375)));
  }

  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 0))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*sequence, Start(testing::_, testing::_, 1))
      .WillOnce(testing::Return(true));

  EXPECT_CALL(*sequence, SampleWeight(2))
      .WillRepeatedly(testing::Return(static_cast<visual_t>(0.5)));

  LowDiscrepancyImageSampler sampler(std::move(sequence), 2);
  sampler.StartPixel({0, 0}, {0, 0});

  random::MockRandom rng;
  EXPECT_TRUE(sampler.NextSample(false, rng));
  EXPECT_TRUE(sampler.NextSample(false, rng));
  EXPECT_FALSE(sampler.NextSample(false, rng));
}

TEST(LowDiscrepancyImageSamplerTest, Replicate) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Duplicate()).WillOnce(testing::Invoke([]() {
    return std::make_unique<MockLowDiscrepancySequence>();
  }));
  LowDiscrepancyImageSampler sampler(std::move(sequence), 2);
  EXPECT_TRUE(sampler.Replicate());
}

}  // namespace
}  // namespace internal
}  // namespace image_samplers
}  // namespace iris
