#include "frontends/pbrt/samplers/zerotwosequence.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::ZeroTwoSequenceSampler;
using ::testing::ExitedWithCode;

TEST(ZeroTwoSequence, Empty) {
  ZeroTwoSequenceSampler zerotwosequence;

  EXPECT_TRUE(MakeZeroTwoSequence(zerotwosequence));
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
