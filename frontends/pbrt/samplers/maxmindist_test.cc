#include "frontends/pbrt/samplers/maxmindist.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::MaxMinDistSampler;
using ::testing::ExitedWithCode;

TEST(MaxMinDist, Empty) {
  MaxMinDistSampler maxmindist;

  EXPECT_TRUE(MakeMaxMinDist(maxmindist));
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
