#include "iris/image_samplers/halton_image_sampler.h"

#include <cstdint>
#include <limits>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace image_samplers {
namespace {

TEST(HaltonImageSamplerTest, Make) {
  EXPECT_TRUE(MakeHaltonImageSampler(0));
  EXPECT_TRUE(MakeHaltonImageSampler(1));
  EXPECT_TRUE(MakeHaltonImageSampler(std::numeric_limits<uint32_t>::max()));
}

}  // namespace
}  // namespace image_samplers
}  // namespace iris
