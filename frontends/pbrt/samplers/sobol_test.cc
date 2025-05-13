#include "frontends/pbrt/samplers/sobol.h"

#include <cstdlib>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::v3::Sampler;
using ::testing::ExitedWithCode;

TEST(Sobol, Empty) {
  Sampler::Sobol sobol;

  EXPECT_TRUE(MakeSobol(sobol));
}

TEST(Sobol, TooLowPixelSamples) {
  Sampler::Sobol sobol;
  sobol.set_pixelsamples(-1);

  EXPECT_EXIT(MakeSobol(sobol), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: pixelsamples");
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
