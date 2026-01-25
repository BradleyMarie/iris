#include "frontends/pbrt/samplers/zerotwosequence.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::v3::Sampler;
using ::testing::ExitedWithCode;

TEST(ZeroTwoSequence, Empty) {
  Sampler::ZeroTwoSequence zerotwosequence;

  EXPECT_TRUE(MakeZeroTwoSequence(zerotwosequence));
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
