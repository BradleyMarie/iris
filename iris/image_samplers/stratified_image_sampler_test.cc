#include "iris/image_samplers/stratified_image_sampler.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

TEST(StraifiedImageSamplerTest, Duplicate) {
  iris::image_samplers::StratifiedImageSampler sampler(5, 3, false);
  EXPECT_TRUE(sampler.Replicate());
}

TEST(StraifiedImageSamplerTest, NoSamples) {
  iris::image_samplers::StratifiedImageSampler sampler(0, 0, false);

  sampler.StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  iris::random::MockRandom rng;
  EXPECT_FALSE(sampler.NextSample(false, rng).has_value());
}

TEST(StraifiedImageSamplerTest, SampleNoLens) {
  iris::image_samplers::StratifiedImageSampler sampler(2, 2, false);

  sampler.StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  iris::random::MockRandom rng;
  auto sample = sampler.NextSample(false, rng);
  EXPECT_EQ(sample->image_uv[0], 0.625);
  EXPECT_EQ(sample->image_uv[1], 0.125);
  EXPECT_NEAR(sample->image_uv[0] + 0.25, sample->image_uv_dxdy[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.25, sample->image_uv_dxdy[1], 0.01);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(sample->weight, 0.25);
  EXPECT_EQ(&rng, &sample->rng);
}

TEST(StraifiedImageSamplerTest, SampleWithLens) {
  iris::image_samplers::StratifiedImageSampler sampler(2, 2, false);

  sampler.StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.1));

  auto sample = sampler.NextSample(true, rng);
  EXPECT_EQ(sample->image_uv[0], 0.625);
  EXPECT_EQ(sample->image_uv[1], 0.125);
  ASSERT_TRUE(sample->lens_uv);
  EXPECT_TRUE((*sample->lens_uv)[0] >= 0.0);
  EXPECT_TRUE((*sample->lens_uv)[0] < 1.0);
  EXPECT_TRUE((*sample->lens_uv)[1] >= 0.0);
  EXPECT_TRUE((*sample->lens_uv)[1] < 1.0);
  EXPECT_NEAR(sample->image_uv[0] + 0.25, sample->image_uv_dxdy[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.25, sample->image_uv_dxdy[1], 0.01);
  EXPECT_EQ(sample->weight, 0.25);
  EXPECT_EQ(&rng, &sample->rng);
}

TEST(StraifiedImageSamplerTest, SampleWithJitter) {
  iris::image_samplers::StratifiedImageSampler sampler(2, 2, true);

  sampler.StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.1));

  auto sample = sampler.NextSample(false, rng);
  EXPECT_TRUE(sample->image_uv[0] >= 0.5);
  EXPECT_TRUE(sample->image_uv[0] < 0.75);
  EXPECT_TRUE(sample->image_uv[1] >= 0.0);
  EXPECT_TRUE(sample->image_uv[1] < 0.25);
  EXPECT_NEAR(sample->image_uv[0] + 0.25, sample->image_uv_dxdy[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.25, sample->image_uv_dxdy[1], 0.01);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(sample->weight, 0.25);
  EXPECT_EQ(&rng, &sample->rng);
}