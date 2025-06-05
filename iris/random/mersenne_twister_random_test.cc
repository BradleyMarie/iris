#include "iris/random/mersenne_twister_random.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace random {
namespace {

TEST(MersenneTwisterRandomTest, NextIndex) {
  std::unique_ptr<Random> rng = MakeMersenneTwisterRandom();
  size_t next = rng->NextIndex(10u);
  EXPECT_GE(next, 0u);
  EXPECT_LT(next, 10u);
}

TEST(MersenneTwisterRandomTest, NextGeometric) {
  std::unique_ptr<Random> rng = MakeMersenneTwisterRandom();
  geometric_t next = rng->NextGeometric();
  EXPECT_GE(next, 0.0);
  EXPECT_LT(next, 1.0);
}

TEST(MersenneTwisterRandomTest, NextVisual) {
  std::unique_ptr<Random> rng = MakeMersenneTwisterRandom();
  visual_t next = rng->NextVisual();
  EXPECT_GE(next, 0.0);
  EXPECT_LT(next, 1.0);
}

TEST(MersenneTwisterRandomTest, Replicate) {
  std::unique_ptr<Random> rng0 = MakeMersenneTwisterRandom();
  std::unique_ptr<Random> rng1 = rng0->Replicate();
  EXPECT_NE(rng0.get(), rng1.get());
  EXPECT_NE(rng0->NextGeometric(), rng1->NextGeometric());
}

}  // namespace
}  // namespace random
}  // namespace iris
