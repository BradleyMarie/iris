#include "frontends/pbrt/samplers/maxmindist.h"

#include <cstdlib>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::v3::Sampler;
using ::testing::ExitedWithCode;

TEST(MaxMinDist, Empty) {
  Sampler::MaxMinDist maxmindist;

  EXPECT_TRUE(MakeMaxMinDist(maxmindist));
}

TEST(MaxMinDist, TooLowPixelSamples) {
  Sampler::MaxMinDist maxmindist;
  maxmindist.set_pixelsamples(-1);

  EXPECT_EXIT(MakeMaxMinDist(maxmindist), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: pixelsamples");
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
