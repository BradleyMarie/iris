#include "iris/bxdfs/microfacet_distribution.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace bxdfs {
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

}  // namespace
}  // namespace bxdfs
}  // namespace iris