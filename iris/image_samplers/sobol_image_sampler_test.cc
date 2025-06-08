#include "iris/image_samplers/sobol_image_sampler.h"

#include <cstdint>
#include <limits>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace image_samplers {
namespace {

TEST(SobolImageSamplerTest, Make) {
  EXPECT_TRUE(MakeSobolImageSampler(0));
  EXPECT_TRUE(MakeSobolImageSampler(1));
  EXPECT_TRUE(MakeSobolImageSampler(std::numeric_limits<uint32_t>::max()));
}

}  // namespace
}  // namespace image_samplers
}  // namespace iris
