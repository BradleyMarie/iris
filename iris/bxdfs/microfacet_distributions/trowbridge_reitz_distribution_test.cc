#include "iris/bxdfs/microfacet_distributions/trowbridge_reitz_distribution.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace bxdfs {
namespace microfacet_distributions {
namespace {

TEST(TrowbridgeReitzDistribution, RoughnessToAlpha) {
  EXPECT_NEAR(0.04726, TrowbridgeReitzDistribution::RoughnessToAlpha(0.0),
              0.001);
  EXPECT_NEAR(1.62142, TrowbridgeReitzDistribution::RoughnessToAlpha(1.0),
              0.001);
}

TEST(TrowbridgeReitzDistribution, D) {
  TrowbridgeReitzDistribution distribution(1.0, 2.0);
  EXPECT_EQ(0.0, distribution.D(Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.15915, distribution.D(Vector(0.0, 0.0, 1.0)), 0.001);
}

TEST(TrowbridgeReitzDistribution, Lambda) {
  TrowbridgeReitzDistribution distribution(1.0, 2.0);
  EXPECT_EQ(0.0, distribution.Lambda(Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.34983, distribution.Lambda(Normalize(Vector(1.0, 2.0, 3.0))),
              0.001);
}

TEST(TrowbridgeReitzDistribution, Sample) {
  TrowbridgeReitzDistribution distribution(1.0, 2.0);

  Vector sample0 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.0, 0.0);
  EXPECT_NEAR(0.0, sample0.x, 0.001);
  EXPECT_NEAR(0.0, sample0.y, 0.001);
  EXPECT_NEAR(1.0, sample0.z, 0.001);

  Vector sample1 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.5, 0.5);
  EXPECT_NEAR(0.7071067, sample1.x, 0.001);
  EXPECT_NEAR(0.0, sample1.y, 0.001);
  EXPECT_NEAR(0.7071067, sample1.z, 0.001);

  Vector sample2 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, 3.0)), 0.5, 0.5);
  EXPECT_NEAR(0.10792, sample2.x, 0.001);
  EXPECT_NEAR(0.86342, sample2.y, 0.001);
  EXPECT_NEAR(0.49279, sample2.z, 0.001);

  Vector sample3 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, -3.0)), 0.5, 0.5);
  EXPECT_NEAR(0.10792, sample3.x, 0.001);
  EXPECT_NEAR(0.86342, sample3.y, 0.001);
  EXPECT_NEAR(-0.4927, sample3.z, 0.001);
}

}  // namespace
}  // namespace microfacet_distributions
}  // namespace bxdfs
}  // namespace iris