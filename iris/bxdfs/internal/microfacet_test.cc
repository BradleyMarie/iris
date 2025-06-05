#include "iris/bxdfs/internal/microfacet.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

class TestMicrofacetDistribution final : public MicrofacetDistribution {
 public:
  TestMicrofacetDistribution(Vector vector = Vector(0.0, 0.0, 1.0))
      : vector_(vector) {}

  visual_t D(const Vector& vector) const override { return 1.0; }

  visual_t Lambda(const Vector& vector) const override { return 1.0; }

  Vector Sample(const Vector& incoming, geometric_t u,
                geometric_t v) const override {
    return vector_;
  }

 private:
  Vector vector_;
};

TEST(MicrofacetDistribution, G) {
  TestMicrofacetDistribution distribution;
  EXPECT_NEAR(1.0 / 3.0,
              distribution.G(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0)),
              0.001);
}

TEST(MicrofacetDistribution, G1) {
  TestMicrofacetDistribution distribution;
  EXPECT_NEAR(1.0 / 2.0, distribution.G1(Vector(0.0, 0.0, 1.0)), 0.001);
}

TEST(MicrofacetDistribution, Pdf) {
  TestMicrofacetDistribution distribution;
  EXPECT_NEAR(0.5,
              distribution.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0)),
              0.001);
  EXPECT_NEAR(0.25,
              distribution.Pdf(Vector(0.0, 0.0, 1.0),
                               Vector(std::sqrt(3.0) / 2.0, 0.0, 0.5)),
              0.001);
}

TEST(TrowbridgeReitzDistribution, DRoughness) {
  TrowbridgeReitzDistribution distribution(0.5, true);
  EXPECT_EQ(0.0, distribution.D(Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.24891, distribution.D(Vector(0.0, 0.0, 1.0)), 0.001);
}

TEST(TrowbridgeReitzDistribution, DAlpha) {
  TrowbridgeReitzDistribution distribution(1.0, 2.0, false);
  EXPECT_EQ(0.0, distribution.D(Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.15915, distribution.D(Vector(0.0, 0.0, 1.0)), 0.001);
}

TEST(TrowbridgeReitzDistribution, LambdaRoughness) {
  TrowbridgeReitzDistribution distribution(0.5, true);
  EXPECT_EQ(0.0, distribution.Lambda(Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.15391, distribution.Lambda(Normalize(Vector(1.0, 2.0, 3.0))),
              0.001);
}

TEST(TrowbridgeReitzDistribution, LambdaAlpha) {
  TrowbridgeReitzDistribution distribution(1.0, 2.0, false);
  EXPECT_EQ(0.0, distribution.Lambda(Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.34983, distribution.Lambda(Normalize(Vector(1.0, 2.0, 3.0))),
              0.001);
}

TEST(TrowbridgeReitzDistribution, SampleRoughness) {
  TrowbridgeReitzDistribution distribution(0.5, true);

  Vector sample0 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.0, 0.0);
  EXPECT_NEAR(0.0, sample0.x, 0.001);
  EXPECT_NEAR(0.0, sample0.y, 0.001);
  EXPECT_NEAR(1.0, sample0.z, 0.001);

  Vector sample1 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.5, 0.5);
  EXPECT_NEAR(-0.7491108, sample1.x, 0.001);
  EXPECT_NEAR(0.0, sample1.y, 0.001);
  EXPECT_NEAR(0.6624447, sample1.z, 0.001);

  Vector sample2 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, 3.0)), 0.5, 0.5);
  EXPECT_NEAR(-0.501254, sample2.x, 0.001);
  EXPECT_NEAR(0.5588885, sample2.y, 0.001);
  EXPECT_NEAR(0.6605966, sample2.z, 0.001);

  Vector sample3 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, -3.0)), 0.5, 0.5);
  EXPECT_NEAR(-0.501254, sample3.x, 0.001);
  EXPECT_NEAR(0.5588885, sample3.y, 0.001);
  EXPECT_NEAR(-0.660596, sample3.z, 0.001);
}

TEST(TrowbridgeReitzDistribution, SampleAlpha) {
  TrowbridgeReitzDistribution distribution(1.0, 2.0, false);

  Vector sample0 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.0, 0.0);
  EXPECT_NEAR(0.0, sample0.x, 0.001);
  EXPECT_NEAR(0.0, sample0.y, 0.001);
  EXPECT_NEAR(1.0, sample0.z, 0.001);

  Vector sample1 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.5, 0.5);
  EXPECT_NEAR(-0.7071067, sample1.x, 0.001);
  EXPECT_NEAR(0.0, sample1.y, 0.001);
  EXPECT_NEAR(0.7071067, sample1.z, 0.001);

  Vector sample2 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, 3.0)), 0.5, 0.5);
  EXPECT_NEAR(-0.3708, sample2.x, 0.001);
  EXPECT_NEAR(0.70828, sample2.y, 0.001);
  EXPECT_NEAR(0.60063, sample2.z, 0.001);

  Vector sample3 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, -3.0)), 0.5, 0.5);
  EXPECT_NEAR(-0.37089, sample3.x, 0.001);
  EXPECT_NEAR(0.708286, sample3.y, 0.001);
  EXPECT_NEAR(-0.600636, sample3.z, 0.001);
}

}  // namespace
}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
