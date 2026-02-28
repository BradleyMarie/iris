#include "frontends/pbrt/samplers/halton.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::HaltonSampler;
using ::testing::ExitedWithCode;

TEST(Halton, Empty) {
  HaltonSampler halton;

  EXPECT_TRUE(MakeHalton(halton));
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
