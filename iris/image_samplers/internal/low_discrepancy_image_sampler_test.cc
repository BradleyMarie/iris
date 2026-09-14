#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/image_samplers/internal/mock_low_discrepancy_sequence.h"
#include "iris/random_bitstreams/mock_random_bitstream.h"

namespace iris {
namespace image_samplers {
namespace internal {
namespace {

using ::iris::random_bitstreams::MockRandomBitstream;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

TEST(LowDiscrepancyImageSamplerTest, NextSampleDesiredReached) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Permute(_)).Times(1);

  LowDiscrepancyImageSampler sampler(std::move(sequence), 0);

  MockRandomBitstream rng;
  sampler.StartPixel({0, 0}, {0, 0}, rng);

  EXPECT_FALSE(sampler.NextSample(false, rng));
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleNone) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Permute(_)).Times(1);
  EXPECT_CALL(*sequence, Start(_, _, 0)).WillOnce(Return(false));

  MockRandomBitstream rng;
  LowDiscrepancyImageSampler sampler(std::move(sequence), 1);
  sampler.StartPixel({0, 0}, {0, 0}, rng);

  EXPECT_FALSE(sampler.NextSample(false, rng));
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleNoLens) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Permute(_)).Times(1);

  {
    InSequence s;
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.75));
  }

  EXPECT_CALL(*sequence, Start(_, _, 0)).WillOnce(Return(true));
  EXPECT_CALL(*sequence, SampleWeight(1)).WillOnce(Return(1.0));

  MockRandomBitstream rng;
  LowDiscrepancyImageSampler sampler(std::move(sequence), 1);
  sampler.StartPixel({1, 1}, {0, 0}, rng);

  auto sample = sampler.NextSample(false, rng);
  ASSERT_TRUE(sample);
  EXPECT_EQ(0.25, sample->image_uv[0]);
  EXPECT_EQ(0.75, sample->image_uv[1]);
  EXPECT_NEAR(sample->image_uv[1] + 1.0, sample->image_uv_dxdy[1], 0.01);
  EXPECT_NEAR(sample->image_uv[0] + 1.0, sample->image_uv_dxdy[0], 0.01);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(1.0, sample->weight);
  EXPECT_TRUE(sample->rng);
}

TEST(LowDiscrepancyImageSamplerTest, NextSampleWithLens) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Permute(_)).Times(1);

  {
    InSequence s;
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.75));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.125));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.375));
  }

  EXPECT_CALL(*sequence, Start(_, _, 0)).WillOnce(Return(true));
  EXPECT_CALL(*sequence, SampleWeight(1)).WillOnce(Return(1.0));

  MockRandomBitstream rng;
  LowDiscrepancyImageSampler sampler(std::move(sequence), 1);
  sampler.StartPixel({1, 1}, {0, 0}, rng);

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
  EXPECT_TRUE(sample->rng);
}

TEST(LowDiscrepancyImageSamplerTest, TwoSamples) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Permute(_)).Times(1);

  {
    InSequence s;
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.75));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.125));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.375));
  }

  EXPECT_CALL(*sequence, Start(_, _, 0)).WillOnce(Return(true));
  EXPECT_CALL(*sequence, Start(_, _, 1)).WillOnce(Return(true));
  EXPECT_CALL(*sequence, SampleWeight(2)).WillRepeatedly(Return(0.5));

  MockRandomBitstream rng;
  LowDiscrepancyImageSampler sampler(std::move(sequence), 2);
  sampler.StartPixel({0, 0}, {0, 0}, rng);

  EXPECT_TRUE(sampler.NextSample(false, rng));
  EXPECT_TRUE(sampler.NextSample(false, rng));
  EXPECT_FALSE(sampler.NextSample(false, rng));
}

TEST(LowDiscrepancyImageSamplerTest, Replicate) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Duplicate()).WillOnce([]() {
    return std::make_unique<MockLowDiscrepancySequence>();
  });
  LowDiscrepancyImageSampler sampler(std::move(sequence), 2);
  EXPECT_TRUE(sampler.Replicate());
}

TEST(LowDiscrepancyRandomTest, Passes) {
  auto sequence = std::make_unique<MockLowDiscrepancySequence>();
  EXPECT_CALL(*sequence, Permute(_)).Times(1);

  {
    InSequence s;
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.75));
    EXPECT_CALL(*sequence, Next()).WillOnce(Return(0.875));
  }

  EXPECT_CALL(*sequence, Start(_, _, 0)).WillOnce(Return(true));
  EXPECT_CALL(*sequence, SampleWeight(1)).WillOnce(Return(1.0));

  MockRandomBitstream rng;
  LowDiscrepancyImageSampler sampler(std::move(sequence), 1);
  sampler.StartPixel({1, 1}, {0, 0}, rng);

  auto sample = sampler.NextSample(false, rng);
  ASSERT_TRUE(sample);
  ASSERT_TRUE(sample->rng);
  EXPECT_EQ(0.875, sample->rng->Next());
}

}  // namespace
}  // namespace internal
}  // namespace image_samplers
}  // namespace iris
