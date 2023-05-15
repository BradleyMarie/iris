#include "iris/image_samplers/random_image_sampler.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

TEST(RandomImageSamplerTest, Replicate) {
  iris::image_samplers::RandomImageSampler sampler(5);
  EXPECT_TRUE(sampler.Replicate());
}

TEST(RandomImageSamplerTest, SampleNoLens) {
  iris::image_samplers::RandomImageSampler sampler(1);

  sampler.StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.1));

  auto sample = sampler.NextSample(false, rng);
  ASSERT_TRUE(sample.has_value());
  EXPECT_TRUE(sample->image_uv[0] >= 0.5);
  EXPECT_TRUE(sample->image_uv[0] < 1.0);
  EXPECT_TRUE(sample->image_uv[1] >= 0.0);
  EXPECT_TRUE(sample->image_uv[1] < 0.5);
  EXPECT_NEAR(sample->image_uv[0] + 0.5, (*sample->image_uv_dxdy)[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.5, (*sample->image_uv_dxdy)[1], 0.01);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(sample->weight, 1.0);
  EXPECT_EQ(&rng, &sample->rng);

  EXPECT_FALSE(sampler.NextSample(false, rng).has_value());
}

TEST(RandomImageSamplerTest, SampleWithLens) {
  iris::image_samplers::RandomImageSampler sampler(1);

  sampler.StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(4)
      .WillRepeatedly(testing::Return(0.1));

  auto sample = sampler.NextSample(true, rng);
  ASSERT_TRUE(sample.has_value());
  EXPECT_TRUE(sample->image_uv[0] >= 0.5);
  EXPECT_TRUE(sample->image_uv[0] < 1.0);
  EXPECT_TRUE(sample->image_uv[1] >= 0.0);
  EXPECT_TRUE(sample->image_uv[1] < 0.5);
  ASSERT_TRUE(sample->lens_uv);
  EXPECT_TRUE((*sample->lens_uv)[0] >= 0.0);
  EXPECT_TRUE((*sample->lens_uv)[0] < 1.0);
  EXPECT_TRUE((*sample->lens_uv)[1] >= 0.0);
  EXPECT_TRUE((*sample->lens_uv)[1] < 1.0);
  EXPECT_NEAR(sample->image_uv[0] + 0.5, (*sample->image_uv_dxdy)[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.5, (*sample->image_uv_dxdy)[1], 0.01);
  EXPECT_EQ(sample->weight, 1.0);
  EXPECT_EQ(&rng, &sample->rng);

  EXPECT_FALSE(sampler.NextSample(true, rng).has_value());
}