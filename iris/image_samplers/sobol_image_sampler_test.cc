#include "iris/image_samplers/sobol_image_sampler.h"

#include <cstdint>
#include <limits>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace image_samplers {
namespace {

TEST(SobolImageSamplerTest, Make) {
  EXPECT_TRUE(MakeSobolImageSampler(0, SobolScrambler::None));
  EXPECT_TRUE(MakeSobolImageSampler(1, SobolScrambler::None));
  EXPECT_TRUE(MakeSobolImageSampler(std::numeric_limits<uint32_t>::max(),
                                    SobolScrambler::None));
}

}  // namespace
}  // namespace image_samplers
}  // namespace iris
