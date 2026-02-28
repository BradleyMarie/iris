#include "frontends/pbrt/samplers/sobol.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::SobolSampler;
using ::testing::ExitedWithCode;

TEST(Sobol, Empty) {
  SobolSampler sobol;

  EXPECT_TRUE(MakeSobol(sobol));
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
