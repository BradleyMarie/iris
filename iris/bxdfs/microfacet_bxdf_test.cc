#include "iris/bxdfs/microfacet_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/helpers/diffuse_bxdf.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
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

  bool IsValid() const override { return true; }
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

  bool IsValid() const override { return true; }
};

TEST(MicrofacetBrdf, Nullptr) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel good_fresnel;
  FresnelDielectric bad_fresnel(-1.0, -1.0);
  EXPECT_TRUE(MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution,
                                 good_fresnel));
  EXPECT_FALSE(MakeMicrofacetBrdf(GetBxdfAllocator(), nullptr, distribution,
                                  good_fresnel));
  EXPECT_FALSE(MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution,
                                  bad_fresnel));
}

TEST(MicrofacetBrdf, SampleDiffuseZero) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetBrdf, SampleDiffuseOppositeBxdfHemispheres) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution(Vector(1.0, 0.0, -1.0));
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetBrdf, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  std::optional<Vector> sample = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), *sample);
}

TEST(MicrofacetBrdf, PdfBTDF) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBrdf, PdfNoIncomingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, PdfDifferentBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(0.0, bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                                  Normalize(Vector(0.0, 1.0, 0.0)),
                                  Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, Pdf) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(0.125,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBrdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0),
                                              Vector(0.0, 0.0, 1.0),
                                              Bxdf::Hemisphere::BTDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, ReflectanceNoZIncoming) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 1.0, 0.0),
                                              Vector(0.0, 0.0, -1.0),
                                              Bxdf::Hemisphere::BRDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, ReflectanceNoZOutgoing) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0),
                                              Vector(0.0, 1.0, 0.0),
                                              Bxdf::Hemisphere::BRDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, ReflectanceOppositeBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0),
                                              Vector(0.0, 0.0, -1.0),
                                              Bxdf::Hemisphere::BRDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBrdf, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel fresnel;
  const Bxdf* bxdf =
      MakeMicrofacetBrdf(GetBxdfAllocator(), &reflector, distribution, fresnel);

  const Reflector* reflectance = bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF,
      testing::GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.0416667, reflectance->Reflectance(1.0), 0.001);
}

TEST(MicrofacetBtdf, Nullptr) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestReflectionFresnel good_fresnel;
  FresnelDielectric bad_fresnel(-1.0, -1.0);
  EXPECT_TRUE(MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0, 2.0,
                                 distribution, good_fresnel));
  EXPECT_FALSE(MakeMicrofacetBtdf(GetBxdfAllocator(), nullptr, 1.0, 2.0,
                                  distribution, good_fresnel));
  EXPECT_FALSE(MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 0.0, 2.0,
                                  distribution, good_fresnel));
  EXPECT_FALSE(MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0, 0.0,
                                  distribution, good_fresnel));
  EXPECT_FALSE(MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0, 2.0,
                                  distribution, bad_fresnel));
}

TEST(MicrofacetBtdf, SampleDiffuseZero) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetBtdf, SampleDiffuseSameBxdfHemispheres) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution(Vector(1.0, 0.0, -1.0));
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetBtdf, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  std::optional<Vector> sample = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(Vector(0.0, 0.0, -1.0), *sample);
}

TEST(MicrofacetBtdf, PdfBTDF) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetBtdf, PdfNoIncomingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, PdfSameBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(0.0, bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                  Normalize(Vector(0.0, 1.0, 0.0)),
                                  Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, PdfNoHalfAngle) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        1.0, distribution, fresnel);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, Pdf) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(2.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetBtdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0),
                                              Vector(0.0, 0.0, -1.0),
                                              Bxdf::Hemisphere::BRDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceNoZIncoming) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 1.0, 0.0),
                                              Vector(0.0, 0.0, 1.0),
                                              Bxdf::Hemisphere::BTDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceNoZOutgoing) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0),
                                              Vector(0.0, 1.0, 0.0),
                                              Bxdf::Hemisphere::BTDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceSameBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0),
                                              Vector(0.0, 0.0, 1.0),
                                              Bxdf::Hemisphere::BTDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, ReflectanceNoHalfAngle) {
  MockReflector reflector;
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        1.0, distribution, fresnel);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0),
                                              Vector(0.0, 0.0, -1.0),
                                              Bxdf::Hemisphere::BTDF,
                                              testing::GetSpectralAllocator()));
}

TEST(MicrofacetBtdf, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  TestMicrofacetDistribution distribution;
  TestTransmissionFresnel fresnel;
  const Bxdf* bxdf = MakeMicrofacetBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                        2.0, distribution, fresnel);

  const Reflector* reflectance = bxdf->ReflectanceDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF,
      testing::GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.1666666, reflectance->Reflectance(1.0), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
