#include "iris/environmental_lights/internal/distribution_1d.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

TEST(Distribution1D, SampleContinuous) {
  std::vector<iris::visual> values = {1.0, 2.0, 3.0, 2.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  iris::Sampler sampler0(rng0);

  auto value0 = dist.SampleContinuous(sampler0);
  EXPECT_EQ(0.25, value0);

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  iris::Sampler sampler1(rng1);

  auto value1 = dist.SampleContinuous(sampler1);
  EXPECT_EQ(0.125, value1);
}

TEST(Distribution1D, SampleContinuousAll) {
  std::vector<iris::visual> values = {1.0, 2.0, 3.0, 2.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  iris::Sampler sampler0(rng0);

  iris::visual_t pdf0;
  size_t offset0;
  auto value0 = dist.SampleContinuous(sampler0, &pdf0, &offset0);
  EXPECT_EQ(0.25, value0);
  EXPECT_EQ(1.0, pdf0);
  EXPECT_EQ(1u, offset0);

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  iris::Sampler sampler1(rng1);

  iris::visual_t pdf1;
  size_t offset1;
  auto value1 = dist.SampleContinuous(sampler1, &pdf1, &offset1);
  EXPECT_EQ(0.125, value1);
  EXPECT_EQ(0.5, pdf1);
  EXPECT_EQ(0u, offset1);
}

TEST(Distribution1D, PdfContinuous) {
  std::vector<iris::visual> values = {1.0, 2.0, 3.0, 2.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  auto value0 = dist.PdfContinuous(0.25);
  EXPECT_EQ(1.0, value0);

  auto value1 = dist.PdfContinuous(0.125);
  EXPECT_EQ(0.5, value1);
}

TEST(Distribution1D, SampleDiscrete) {
  std::vector<iris::visual> values = {1.0, 2.0, 3.0, 2.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  iris::Sampler sampler0(rng0);

  auto value0 = dist.SampleDiscrete(sampler0);
  EXPECT_EQ(1u, value0);

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  iris::Sampler sampler1(rng1);

  auto value1 = dist.SampleDiscrete(sampler1);
  EXPECT_EQ(0u, value1);
}

TEST(Distribution1D, SampleDiscreteAll) {
  std::vector<iris::visual> values = {1.0, 2.0, 3.0, 2.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.125));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  iris::Sampler sampler0(rng0);

  iris::visual_t pdf0;
  auto value0 = dist.SampleDiscrete(sampler0, &pdf0);
  EXPECT_EQ(1u, value0);
  EXPECT_EQ(0.25, pdf0);

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  iris::Sampler sampler1(rng1);

  iris::visual_t pdf1;
  auto value1 = dist.SampleDiscrete(sampler1, &pdf1);
  EXPECT_EQ(0u, value1);
  EXPECT_EQ(0.125, pdf1);
}

TEST(Distribution1D, PdfDiscrete) {
  std::vector<iris::visual> values = {1.0, 2.0, 3.0, 2.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  auto value0 = dist.PdfDiscrete(1u);
  EXPECT_EQ(0.25, value0);

  auto value1 = dist.PdfDiscrete(0u);
  EXPECT_EQ(0.125, value1);
}

TEST(Distribution1D, AllZeroSampleContinuous) {
  std::vector<iris::visual> values = {0.0, 0.0, 0.0, 0.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.25));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  iris::Sampler sampler0(rng0);

  iris::visual_t pdf0;
  size_t offset0;
  auto value0 = dist.SampleContinuous(sampler0, &pdf0, &offset0);
  EXPECT_EQ(0.25, value0);
  EXPECT_EQ(0.0, pdf0);
  EXPECT_EQ(1u, offset0);

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  iris::Sampler sampler1(rng1);

  iris::visual_t pdf1;
  size_t offset1;
  auto value1 = dist.SampleContinuous(sampler1, &pdf1, &offset1);
  EXPECT_EQ(0.0625, value1);
  EXPECT_EQ(0.0, pdf1);
  EXPECT_EQ(0u, offset1);
}

TEST(Distribution1D, AllZeroPdfContinuous) {
  std::vector<iris::visual> values = {0.0, 0.0, 0.0, 0.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  auto value0 = dist.PdfContinuous(0.25);
  EXPECT_EQ(0.0, value0);

  auto value1 = dist.PdfContinuous(0.125);
  EXPECT_EQ(0.0, value1);
}

TEST(Distribution1D, AllZeroSampleDiscrete) {
  std::vector<iris::visual> values = {0.0, 0.0, 0.0, 0.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  iris::random::MockRandom rng0;
  EXPECT_CALL(rng0, NextGeometric()).WillOnce(testing::Return(0.25));
  EXPECT_CALL(rng0, DiscardGeometric(1));
  iris::Sampler sampler0(rng0);

  iris::visual_t pdf0;
  auto value0 = dist.SampleDiscrete(sampler0, &pdf0);
  EXPECT_EQ(1u, value0);
  EXPECT_EQ(0.0, pdf0);

  iris::random::MockRandom rng1;
  EXPECT_CALL(rng1, NextGeometric()).WillOnce(testing::Return(0.0625));
  EXPECT_CALL(rng1, DiscardGeometric(1));
  iris::Sampler sampler1(rng1);

  iris::visual_t pdf1;
  auto value1 = dist.SampleDiscrete(sampler1, &pdf1);
  EXPECT_EQ(0u, value1);
  EXPECT_EQ(0.0, pdf1);
}

TEST(Distribution1D, AllZeroPdfDiscrete) {
  std::vector<iris::visual> values = {0.0, 0.0, 0.0, 0.0};
  iris::environmental_lights::internal::Distribution1D dist(values);

  auto value0 = dist.PdfDiscrete(1u);
  EXPECT_EQ(0.0, value0);

  auto value1 = dist.PdfDiscrete(0u);
  EXPECT_EQ(0.0, value1);
}