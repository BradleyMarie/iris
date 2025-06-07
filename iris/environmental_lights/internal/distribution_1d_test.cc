#include "iris/environmental_lights/internal/distribution_1d.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace environmental_lights {
namespace internal {
namespace {

using ::iris::random::MockRandom;
using ::testing::InSequence;
using ::testing::Return;

TEST(Distribution1D, SampleContinuous) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  Sampler sampler0(rng0);

  geometric_t value0 = dist.SampleContinuous(sampler0);
  EXPECT_EQ(0.25, value0);

  MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  Sampler sampler1(rng1);

  geometric_t value1 = dist.SampleContinuous(sampler1);
  EXPECT_EQ(0.125, value1);
}

TEST(Distribution1D, SampleContinuousAll) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  Sampler sampler0(rng0);

  visual_t pdf0;
  size_t offset0;
  geometric_t value0 = dist.SampleContinuous(sampler0, &pdf0, &offset0);
  EXPECT_EQ(0.25, value0);
  EXPECT_EQ(1.0, pdf0);
  EXPECT_EQ(1u, offset0);

  MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  Sampler sampler1(rng1);

  visual_t pdf1;
  size_t offset1;
  geometric_t value1 = dist.SampleContinuous(sampler1, &pdf1, &offset1);
  EXPECT_EQ(0.125, value1);
  EXPECT_EQ(0.5, pdf1);
  EXPECT_EQ(0u, offset1);
}

TEST(Distribution1D, PdfContinuous) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfContinuous(0.25);
  EXPECT_EQ(1.0, value0);

  geometric_t value1 = dist.PdfContinuous(0.125);
  EXPECT_EQ(0.5, value1);
}

TEST(Distribution1D, SampleDiscrete) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  Sampler sampler0(rng0);

  geometric_t value0 = dist.SampleDiscrete(sampler0);
  EXPECT_EQ(1u, value0);

  MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  Sampler sampler1(rng1);

  geometric_t value1 = dist.SampleDiscrete(sampler1);
  EXPECT_EQ(0u, value1);
}

TEST(Distribution1D, SampleDiscreteAll) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  Sampler sampler0(rng0);

  visual_t pdf0;
  geometric_t value0 = dist.SampleDiscrete(sampler0, &pdf0);
  EXPECT_EQ(1u, value0);
  EXPECT_EQ(0.25, pdf0);

  MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  Sampler sampler1(rng1);

  visual_t pdf1;
  geometric_t value1 = dist.SampleDiscrete(sampler1, &pdf1);
  EXPECT_EQ(0u, value1);
  EXPECT_EQ(0.125, pdf1);
}

TEST(Distribution1D, PdfDiscrete) {
  std::vector<visual> values = {1.0, 2.0, 3.0, 2.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfDiscrete(1u);
  EXPECT_EQ(0.25, value0);

  geometric_t value1 = dist.PdfDiscrete(0u);
  EXPECT_EQ(0.125, value1);
}

TEST(Distribution1D, AllZeroSampleContinuous) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(Return(0.25));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  Sampler sampler0(rng0);

  visual_t pdf0;
  size_t offset0;
  geometric_t value0 = dist.SampleContinuous(sampler0, &pdf0, &offset0);
  EXPECT_EQ(0.25, value0);
  EXPECT_EQ(0.0, pdf0);
  EXPECT_EQ(1u, offset0);

  MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  Sampler sampler1(rng1);

  visual_t pdf1;
  size_t offset1;
  geometric_t value1 = dist.SampleContinuous(sampler1, &pdf1, &offset1);
  EXPECT_EQ(0.0625, value1);
  EXPECT_EQ(0.0, pdf1);
  EXPECT_EQ(0u, offset1);
}

TEST(Distribution1D, AllZeroPdfContinuous) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfContinuous(0.25);
  EXPECT_EQ(0.0, value0);

  geometric_t value1 = dist.PdfContinuous(0.125);
  EXPECT_EQ(0.0, value1);
}

TEST(Distribution1D, AllZeroSampleDiscrete) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(Return(0.25));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  Sampler sampler0(rng0);

  visual_t pdf0;
  geometric_t value0 = dist.SampleDiscrete(sampler0, &pdf0);
  EXPECT_EQ(1u, value0);
  EXPECT_EQ(0.0, pdf0);

  MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  Sampler sampler1(rng1);

  visual_t pdf1;
  geometric_t value1 = dist.SampleDiscrete(sampler1, &pdf1);
  EXPECT_EQ(0u, value1);
  EXPECT_EQ(0.0, pdf1);
}

TEST(Distribution1D, AllZeroPdfDiscrete) {
  std::vector<visual> values = {0.0, 0.0, 0.0, 0.0};
  Distribution1D dist(values);

  geometric_t value0 = dist.PdfDiscrete(1u);
  EXPECT_EQ(0.0, value0);

  geometric_t value1 = dist.PdfDiscrete(0u);
  EXPECT_EQ(0.0, value1);
}

TEST(Distribution1D, SampleContinuousWithZeroes) {
  std::vector<visual> values = {0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0};
  Distribution1D dist(values);

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
    EXPECT_CALL(rng, DiscardGeometric(1));
    Sampler sampler(rng);

    size_t offset;
    geometric_t sample = dist.SampleContinuous(sampler, nullptr, &offset);
    EXPECT_EQ(0.125, sample);
    EXPECT_EQ(1u, offset);
  }

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
    EXPECT_CALL(rng, DiscardGeometric(1));
    Sampler sampler(rng);

    size_t offset;
    geometric_t sample = dist.SampleContinuous(sampler, nullptr, &offset);
    EXPECT_EQ(0.375, sample);
    EXPECT_EQ(3u, offset);
  }

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.9999999));
    EXPECT_CALL(rng, DiscardGeometric(1));
    Sampler sampler(rng);

    size_t offset;
    geometric_t sample = dist.SampleContinuous(sampler, nullptr, &offset);
    EXPECT_NEAR(0.875, sample, 0.001);
    EXPECT_EQ(6u, offset);
  }
}

TEST(Distribution1D, SampleDiscreteWithZeroes) {
  std::vector<visual> values = {0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0};
  Distribution1D dist(values);

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
    EXPECT_CALL(rng, DiscardGeometric(1));
    Sampler sampler(rng);

    size_t sample = dist.SampleDiscrete(sampler);
    EXPECT_EQ(1u, sample);
  }

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
    EXPECT_CALL(rng, DiscardGeometric(1));
    Sampler sampler(rng);

    size_t sample = dist.SampleDiscrete(sampler);
    EXPECT_EQ(3u, sample);
  }

  {
    InSequence sequence;

    MockRandom rng;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.9999999));
    EXPECT_CALL(rng, DiscardGeometric(1));
    Sampler sampler(rng);

    size_t sample = dist.SampleDiscrete(sampler);
    EXPECT_EQ(6u, sample);
  }
}

}  // namespace
}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris
