#include "iris/image_samplers/random_image_sampler.h"

#include <optional>
#include <utility>

#include "googletest/include/gtest/gtest.h"
#include "iris/image_sampler.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace image_samplers {
namespace {

using ::iris::random::MockRandom;
using ::testing::Return;

TEST(RandomImageSamplerTest, Replicate) {
  std::unique_ptr<ImageSampler> sampler = MakeRandomImageSampler(5);
  EXPECT_TRUE(sampler->Replicate());
}

TEST(RandomImageSamplerTest, SampleNoLens) {
  std::unique_ptr<ImageSampler> sampler = MakeRandomImageSampler(1);

  sampler->StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.1));

  std::optional<ImageSampler::Sample> sample = sampler->NextSample(false, rng);
  ASSERT_TRUE(sample.has_value());
  EXPECT_TRUE(sample->image_uv[0] >= 0.5);
  EXPECT_TRUE(sample->image_uv[0] < 1.0);
  EXPECT_TRUE(sample->image_uv[1] >= 0.0);
  EXPECT_TRUE(sample->image_uv[1] < 0.5);
  EXPECT_NEAR(sample->image_uv[0] + 0.5, sample->image_uv_dxdy[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.5, sample->image_uv_dxdy[1], 0.01);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(sample->weight, 1.0);
  EXPECT_EQ(&rng, &sample->rng);

  EXPECT_FALSE(sampler->NextSample(false, rng).has_value());
}

TEST(RandomImageSamplerTest, SampleWithLens) {
  std::unique_ptr<ImageSampler> sampler = MakeRandomImageSampler(1);

  sampler->StartPixel(std::make_pair(2, 2), std::make_pair(0, 1));

  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(4).WillRepeatedly(Return(0.1));

  std::optional<ImageSampler::Sample> sample = sampler->NextSample(true, rng);
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
  EXPECT_NEAR(sample->image_uv[0] + 0.5, sample->image_uv_dxdy[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.5, sample->image_uv_dxdy[1], 0.01);
  EXPECT_EQ(sample->weight, 1.0);
  EXPECT_EQ(&rng, &sample->rng);

  EXPECT_FALSE(sampler->NextSample(true, rng).has_value());
}

}  // namespace
}  // namespace image_samplers
}  // namespace iris
