#include "iris/image_samplers/stratified_image_sampler.h"

#include <optional>
#include <utility>

#include "googletest/include/gtest/gtest.h"
#include "iris/image_sampler.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace image_samplers {
namespace {

using ::iris::random::MockRandom;

TEST(StraifiedImageSamplerTest, Duplicate) {
  std::unique_ptr<ImageSampler> sampler =
      MakeStratifiedImageSampler(5, 3, false);
  EXPECT_TRUE(sampler->Replicate());
}

TEST(StraifiedImageSamplerTest, NoSamples) {
  std::unique_ptr<ImageSampler> sampler =
      MakeStratifiedImageSampler(0, 0, false);

  MockRandom rng;
  sampler->StartPixel(std::make_pair(2, 2), std::make_pair(0, 1), rng);

  EXPECT_FALSE(sampler->NextSample(false, rng).has_value());
}

TEST(StraifiedImageSamplerTest, SampleNoLens) {
  std::unique_ptr<ImageSampler> sampler =
      MakeStratifiedImageSampler(2, 2, false);

  MockRandom rng;
  sampler->StartPixel(std::make_pair(2, 2), std::make_pair(0, 1), rng);

  std::optional<ImageSampler::Sample> sample = sampler->NextSample(false, rng);
  EXPECT_EQ(sample->image_uv[0], 0.625);
  EXPECT_EQ(sample->image_uv[1], 0.125);
  EXPECT_NEAR(sample->image_uv[0] + 0.25, sample->image_uv_dxdy[0], 0.01);
  EXPECT_NEAR(sample->image_uv[1] + 0.25, sample->image_uv_dxdy[1], 0.01);
  EXPECT_FALSE(sample->lens_uv);
  EXPECT_EQ(sample->weight, 0.25);
  EXPECT_EQ(&rng, &sample->rng);
}

TEST(StraifiedImageSamplerTest, SampleWithLens) {
  std::unique_ptr<ImageSampler> sampler =
      MakeStratifiedImageSampler(2, 2, false);

  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.1));

  sampler->StartPixel(std::make_pair(2, 2), std::make_pair(0, 1), rng);

  std::optional<ImageSampler::Sample> sample = sampler->NextSample(true, rng);
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
  std::unique_ptr<ImageSampler> sampler =
      MakeStratifiedImageSampler(2, 2, true);

  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.1));

  sampler->StartPixel(std::make_pair(2, 2), std::make_pair(0, 1), rng);

  std::optional<ImageSampler::Sample> sample = sampler->NextSample(false, rng);
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

}  // namespace
}  // namespace image_samplers
}  // namespace iris
