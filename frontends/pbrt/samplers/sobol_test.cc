#include "frontends/pbrt/samplers/sobol.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/v3.pb.h"

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

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
