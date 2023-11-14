#include "iris/image_samplers/sobol_image_sampler.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"

TEST(SobolImageSamplerTest, Make) {
  EXPECT_TRUE(iris::image_samplers::MakeSobolImageSampler(0));
  EXPECT_TRUE(iris::image_samplers::MakeSobolImageSampler(1));
  EXPECT_TRUE(iris::image_samplers::MakeSobolImageSampler(
      std::numeric_limits<uint32_t>::max()));
}