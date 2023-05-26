#include "iris/bxdfs/microfacet_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/testing/spectral_allocator.h"

class TestMicrofacetDistribution final
    : public iris::bxdfs::MicrofacetDistribution {
 public:
  TestMicrofacetDistribution(iris::Vector vector = iris::Vector(0.0, 0.0, 1.0))
      : vector_(vector) {}

  iris::visual_t D(const iris::Vector& vector) const override { return 1.0; }

  iris::visual_t Lambda(const iris::Vector& vector) const override {
    return 1.0;
  }

  iris::Vector Sample(const iris::Vector& incoming, iris::geometric_t u,
                      iris::geometric_t v) const override {
    return vector_;
  }

 private:
  iris::Vector vector_;
};

class TestFresnel final : public iris::bxdfs::Fresnel {
 public:
  const iris::Reflector* Evaluate(
      iris::visual_t cos_theta_incident,
      iris::SpectralAllocator& allocator) const override {
    static const iris::reflectors::UniformReflector perfect_reflector(1.0);
    return &perfect_reflector;
  }
};

TEST(MicrofacetDistribution, G) {
  TestMicrofacetDistribution distribution;
  EXPECT_NEAR(
      1.0 / 3.0,
      distribution.G(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0)),
      0.001);
}

TEST(MicrofacetDistribution, G1) {
  TestMicrofacetDistribution distribution;
  EXPECT_NEAR(1.0 / 2.0, distribution.G1(iris::Vector(0.0, 0.0, 1.0)), 0.001);
}

TEST(MicrofacetDistribution, Pdf) {
  TestMicrofacetDistribution distribution;
  EXPECT_NEAR(0.5,
              distribution.Pdf(iris::Vector(0.0, 0.0, 1.0),
                               iris::Vector(0.0, 0.0, 1.0)),
              0.001);
  EXPECT_NEAR(0.25,
              distribution.Pdf(iris::Vector(0.0, 0.0, 1.0),
                               iris::Vector(std::sqrt(3.0) / 2.0, 0.0, 0.5)),
              0.001);
}

TEST(TrowbridgeReitzDistribution, RoughnessToAlpha) {
  EXPECT_NEAR(0.04726,
              iris::bxdfs::TrowbridgeReitzDistribution::RoughnessToAlpha(0.0),
              0.001);
  EXPECT_NEAR(1.62142,
              iris::bxdfs::TrowbridgeReitzDistribution::RoughnessToAlpha(1.0),
              0.001);
}

TEST(TrowbridgeReitzDistribution, D) {
  iris::bxdfs::TrowbridgeReitzDistribution distribution(1.0, 2.0);
  EXPECT_EQ(0.0, distribution.D(iris::Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.15915, distribution.D(iris::Vector(0.0, 0.0, 1.0)), 0.001);
}

TEST(TrowbridgeReitzDistribution, Lambda) {
  iris::bxdfs::TrowbridgeReitzDistribution distribution(1.0, 2.0);
  EXPECT_EQ(0.0, distribution.Lambda(iris::Vector(0.0, 1.0, 0.0)));
  EXPECT_NEAR(0.34983,
              distribution.Lambda(iris::Normalize(iris::Vector(1.0, 2.0, 3.0))),
              0.001);
}

TEST(TrowbridgeReitzDistribution, Sample) {
  iris::bxdfs::TrowbridgeReitzDistribution distribution(1.0, 2.0);

  auto sample0 = distribution.Sample(iris::Vector(0.0, 0.0, 1.0), 0.0, 0.0);
  EXPECT_NEAR(0.0, sample0.x, 0.001);
  EXPECT_NEAR(0.0, sample0.y, 0.001);
  EXPECT_NEAR(1.0, sample0.z, 0.001);

  auto sample1 = distribution.Sample(iris::Vector(0.0, 0.0, 1.0), 0.5, 0.5);
  EXPECT_NEAR(0.7071067, sample1.x, 0.001);
  EXPECT_NEAR(0.0, sample1.y, 0.001);
  EXPECT_NEAR(0.7071067, sample1.z, 0.001);

  auto sample2 = distribution.Sample(
      iris::Normalize(iris::Vector(1.0, 2.0, 3.0)), 0.5, 0.5);
  EXPECT_NEAR(0.10792, sample2.x, 0.001);
  EXPECT_NEAR(0.86342, sample2.y, 0.001);
  EXPECT_NEAR(0.49279, sample2.z, 0.001);
}

TEST(MicrofacetBrdf, SampleZero) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution;
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_FALSE(brdf.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt, sampler));
}

TEST(MicrofacetBrdf, SampleZeroDp) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.75));
  iris::Sampler sampler(rng);

  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution(iris::Vector(0.0, 0.0, -1.0));
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_FALSE(brdf.Sample(iris::Vector(0.0, 0.0, 1.0), std::nullopt, sampler));
}

TEST(MicrofacetBrdf, SampleOppositeHemispheres) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.75));
  iris::Sampler sampler(rng);

  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution(iris::Vector(1.0, 0.0, -1.0));
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_FALSE(brdf.Sample(iris::Vector(1.0, 0.0, 1.0), std::nullopt, sampler));
}

TEST(MicrofacetBrdf, Sample) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.75));
  iris::Sampler sampler(rng);

  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution;
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  auto sample = brdf.Sample(iris::Vector(0.0, 0.0, 1.0), std::nullopt, sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), sample->direction);
  EXPECT_FALSE(sample->differentials);
}

TEST(MicrofacetBrdf, PdfOpposite) {
  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution;
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(0.0,
            brdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
                     iris::Bxdf::SampleSource::BXDF)
                .value());
}

TEST(MicrofacetBrdf, Pdf) {
  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution;
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(0.125,
            brdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                     iris::Bxdf::SampleSource::BXDF)
                .value());
}

TEST(MicrofacetBrdf, ReflectanceWrongHemishphere) {
  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution;
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(nullptr, brdf.Reflectance(iris::Vector(0.0, 0.0, 1.0),
                                      iris::Vector(0.0, 0.0, 1.0),
                                      iris::Bxdf::SampleSource::BXDF,
                                      iris::Bxdf::Hemisphere::BTDF,
                                      iris::testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, ReflectanceOppositeHemispheres) {
  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution;
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(nullptr, brdf.Reflectance(iris::Vector(0.0, 0.0, 1.0),
                                      iris::Vector(0.0, 0.0, -1.0),
                                      iris::Bxdf::SampleSource::BXDF,
                                      iris::Bxdf::Hemisphere::BRDF,
                                      iris::testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, Reflectance) {
  iris::reflectors::UniformReflector reflector(0.5);
  TestMicrofacetDistribution distribution;
  TestFresnel fresnel;
  iris::bxdfs::MicrofacetBrdf brdf(reflector, distribution, fresnel);

  const auto* reflectance = brdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Bxdf::SampleSource::BXDF, iris::Bxdf::Hemisphere::BRDF,
      iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.0416667, reflectance->Reflectance(1.0), 0.001);
}