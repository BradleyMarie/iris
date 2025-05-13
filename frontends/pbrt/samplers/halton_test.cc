#include "frontends/pbrt/samplers/halton.h"

#include <cstdlib>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::v3::Sampler;
using ::testing::ExitedWithCode;

TEST(Halton, Empty) {
  Sampler::Halton halton;

  EXPECT_TRUE(MakeHalton(halton));
}

TEST(Halton, TooLowPixelSamples) {
  Sampler::Halton halton;
  halton.set_pixelsamples(-1);

  EXPECT_EXIT(MakeHalton(halton), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: pixelsamples");
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
