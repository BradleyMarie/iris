#include "iris/image_samplers/halton_image_sampler.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"

TEST(HaltonImageSamplerTest, Make) {
  EXPECT_TRUE(iris::image_samplers::MakeHaltonImageSampler(0));
  EXPECT_TRUE(iris::image_samplers::MakeHaltonImageSampler(1));
  EXPECT_TRUE(iris::image_samplers::MakeHaltonImageSampler(
      std::numeric_limits<uint32_t>::max()));
}