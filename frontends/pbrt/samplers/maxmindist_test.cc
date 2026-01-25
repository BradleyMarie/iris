#include "frontends/pbrt/samplers/maxmindist.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/v3.pb.h"

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

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
