#include "iris/random_bitstreams/mersenne_twister_random_bitstream.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/random_bitstream.h"

namespace iris {
namespace random_bitstreams {
namespace {

TEST(MersenneTwisterRandomBitstreamTest, Replicate) {
  std::unique_ptr<RandomBitstream> rng0 = MakeMersenneTwisterRandomBitstream();
  std::unique_ptr<RandomBitstream> rng1 = rng0->Replicate();
  EXPECT_NE(rng0.get(), rng1.get());
  EXPECT_NE(rng0->NextGeometric(), rng1->NextGeometric());
}

}  // namespace
}  // namespace random_bitstreams
}  // namespace iris
