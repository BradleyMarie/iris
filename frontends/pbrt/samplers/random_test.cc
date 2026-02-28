#include "frontends/pbrt/samplers/random.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::IndependentSampler;
using ::testing::ExitedWithCode;

TEST(Random, Empty) {
  IndependentSampler random;

  EXPECT_TRUE(MakeRandom(random));
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
