#include "iris/bxdfs/microfacet_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::testing::_;
using ::testing::Return;

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

class TestReflectionFresnel final : public Fresnel {
 public:
  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return &reflectance;
  }

  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return nullptr;
  }
};

class TestTransmissionFresnel final : public Fresnel {
 public:
  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return nullptr;
  }

  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return &transmittance;
  }
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

  auto sample0 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.0, 0.0);
  EXPECT_NEAR(0.0, sample0.x, 0.001);
  EXPECT_NEAR(0.0, sample0.y, 0.001);
  EXPECT_NEAR(1.0, sample0.z, 0.001);

  auto sample1 = distribution.Sample(Vector(0.0, 0.0, 1.0), 0.5, 0.5);
  EXPECT_NEAR(0.7071067, sample1.x, 0.001);
  EXPECT_NEAR(0.0, sample1.y, 0.001);
  EXPECT_NEAR(0.7071067, sample1.z, 0.001);

  auto sample2 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, 3.0)), 0.5, 0.5);
  EXPECT_NEAR(0.10792, sample2.x, 0.001);
  EXPECT_NEAR(0.86342, sample2.y, 0.001);
  EXPECT_NEAR(0.49279, sample2.z, 0.001);

  auto sample3 =
      distribution.Sample(Normalize(Vector(1.0, 2.0, -3.0)), 0.5, 0.5);
  EXPECT_NEAR(0.10792, sample3.x, 0.001);
  EXPECT_NEAR(0.86342, sample3.y, 0.001);
  EXPECT_NEAR(-0.4927, sample3.z, 0.001);
}

TEST(MicrofacetBrdf, IsDiffuse) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);
  EXPECT_TRUE(brdf.IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_TRUE(brdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(1.0, diffuse_pdf);
}

TEST(MicrofacetBrdf, SampleDiffuseZero) {
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_FALSE(brdf.SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0),
                                  sampler));
}

TEST(MicrofacetBrdf, SampleDiffuseOppositeHemispheres) {
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution(Vector(1.0, 0.0, -1.0));
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_FALSE(brdf.SampleDiffuse(Vector(1.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                  sampler));
}

TEST(MicrofacetBrdf, SampleDiffuse) {
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  auto sample =
      brdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), *sample);
}

TEST(MicrofacetBrdf, Sample) {
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  auto sample = brdf.Sample(Vector(0.0, 0.0, 1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), sample->direction);
  EXPECT_FALSE(sample->differentials);
  EXPECT_EQ(sample->bxdf_override, nullptr);
  EXPECT_EQ(sample->pdf_weight, 1.0);
}

TEST(MicrofacetBrdf, PdfBTDF) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(0.0, brdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBrdf, PdfNoIncomingZ) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(0.0, brdf.Pdf(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, PdfNoOutgoingZ) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(0.0, brdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, PdfDifferentHemispheres) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(0.0,
            brdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                     Normalize(Vector(0.0, 1.0, 0.0)), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, Pdf) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(0.125, brdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, ReflectanceWrongHemishphere) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(nullptr,
            brdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                             Bxdf::Hemisphere::BTDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, ReflectanceNoZIncoming) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(nullptr,
            brdf.Reflectance(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                             Bxdf::Hemisphere::BRDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, ReflectanceNoZOutgoing) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(nullptr,
            brdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                             Bxdf::Hemisphere::BRDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, ReflectanceOppositeHemispheres) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  EXPECT_EQ(nullptr,
            brdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Bxdf::Hemisphere::BRDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, Reflectance) {
  reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  MicrofacetBrdf brdf(reflector, distribution, fresnel);

  const auto* reflectance =
      brdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BRDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.0416667, reflectance->Reflectance(1.0), 0.001);
}

TEST(MicrofacetBtdf, IsDiffuse) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);
  EXPECT_TRUE(btdf.IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_TRUE(btdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(1.0, diffuse_pdf);
}

TEST(MicrofacetBtdf, SampleDiffuseZero) {
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_FALSE(btdf.SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0),
                                  sampler));
}

TEST(MicrofacetBtdf, SampleDiffuseSameHemispheres) {
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution(Vector(1.0, 0.0, -1.0));
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_FALSE(btdf.SampleDiffuse(Vector(1.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                  sampler));
}

TEST(MicrofacetBtdf, SampleDiffuse) {
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  auto sample =
      btdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(Vector(0.0, 0.0, -1.0), *sample);
}

TEST(MicrofacetBtdf, Sample) {
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  auto sample = btdf.Sample(Vector(0.0, 0.0, 1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(Vector(0.0, 0.0, -1.0), sample->direction);
  EXPECT_FALSE(sample->differentials);
  EXPECT_EQ(sample->pdf_weight, 1.0);
}

TEST(MicrofacetBtdf, PdfBTDF) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(0.0, btdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBtdf, PdfNoIncomingZ) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(0.0, btdf.Pdf(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                          Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, PdfNoOutgoingZ) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(0.0, btdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                          Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, PdfSameHemispheres) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(0.0,
            btdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                     Normalize(Vector(0.0, 1.0, 0.0)), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, PdfNoHalfAngle) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 1.0, distribution, fresnel);

  EXPECT_EQ(0.0, btdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, Pdf) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(2.0, btdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, ReflectanceWrongHemishphere) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(nullptr,
            btdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Bxdf::Hemisphere::BRDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceNoZIncoming) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(nullptr,
            btdf.Reflectance(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                             Bxdf::Hemisphere::BTDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceNoZOutgoing) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(nullptr,
            btdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                             Bxdf::Hemisphere::BTDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceSameHemispheres) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  EXPECT_EQ(nullptr,
            btdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                             Bxdf::Hemisphere::BTDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceNoHalfAngle) {
  reflectors::MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 1.0, distribution, fresnel);

  EXPECT_EQ(nullptr,
            btdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Bxdf::Hemisphere::BTDF,
                             testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, Reflectance) {
  reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  MicrofacetBtdf btdf(reflector, 1.0, 2.0, distribution, fresnel);

  const auto* reflectance =
      btdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                       Bxdf::Hemisphere::BTDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.1666666, reflectance->Reflectance(1.0), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris