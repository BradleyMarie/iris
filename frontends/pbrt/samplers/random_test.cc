#include "frontends/pbrt/samplers/random.h"

#include <cstdlib>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::v3::Sampler;
using ::testing::ExitedWithCode;

TEST(Random, Empty) {
  Sampler::Random random;

  EXPECT_TRUE(MakeRandom(random));
}

TEST(Random, TooLowPixelSamples) {
  Sampler::Random random;
  random.set_pixelsamples(-1);

  EXPECT_EXIT(MakeRandom(random), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: pixelsamples");
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
