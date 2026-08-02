#include "iris/bxdfs/internal/microfacet_bxdf.h"

#include <optional>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/bxdf.h"
#include "iris/bxdfs/internal/fresnel.h"
#include "iris/bxdfs/internal/microfacet_distribution.h"
#include "iris/float.h"
#include "iris/random/mock_random.h"
#include "iris/reflector.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

const static class TestMicrofacetDistribution : public MicrofacetDistribution {
 public:
  visual_t D(const Vector& vector) const override { return 1.0; }

  visual_t Lambda(const Vector& vector) const override { return 1.0; }

  Vector Sample(const Vector& incoming, geometric_t u,
                geometric_t v) const override {
    return Vector(0.0, 0.0, 1.0);
  }
} kDistribution;

const static class TestFresnel : public Fresnel {
 public:
  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return &reflectance;
  }

  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override {
    return &transmittance;
  }

  bool IsValid() const override { return true; }
} kFresnel;

typedef MicrofacetBrdf<TestMicrofacetDistribution, TestFresnel>
    TestMicrofacetBrdf;
typedef MicrofacetBtdf<TestMicrofacetDistribution> TestMicrofacetBtdf;

TEST(TestMicrofacetBrdf, SampleDiffuseZero) {
  MockRandom rng;
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(TestMicrofacetBrdf, SampleDiffuseOppositeBxdfHemispheres) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(1.0));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  std::optional<Vector> result = bxdf.SampleDiffuse(
      Vector(1.0, 0.0, -1.0), Vector(0.0, 0.0, -1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(TestMicrofacetBrdf, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.5));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.5, 0.5, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.5, 0.001);
  EXPECT_NEAR(result->y, -0.5, 0.001);
  EXPECT_NEAR(result->z, 1.0, 0.001);
}

TEST(TestMicrofacetBrdf, PdfBTDF) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(TestMicrofacetBrdf, PdfNoIncomingZ) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(TestMicrofacetBrdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(TestMicrofacetBrdf, PdfDifferentBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(TestMicrofacetBrdf, Pdf) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_NEAR(0.125,
              bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                              Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.0001);
}

TEST(TestMicrofacetBrdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBrdf, ReflectanceNoZIncoming) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBrdf, ReflectanceNoZOutgoing) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBrdf, ReflectanceOppositeBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBrdf, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.00726));
  TestMicrofacetBrdf bxdf(reflector, kDistribution, kFresnel);

  const Reflector* reflectance =
      bxdf.ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                              Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.000605, reflectance->Reflectance(1.0), 0.00001);
}

TEST(TestMicrofacetBtdf, SampleDiffuseZero) {
  MockRandom rng;
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(TestMicrofacetBtdf, SampleDiffuseSameBxdfHemispheres) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(1.0));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 1.0);

  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, 0.0, 0.001);
  EXPECT_NEAR(result->y, 0.0, 0.001);
  EXPECT_NEAR(result->z, -1.0, 0.001);
}

TEST(TestMicrofacetBtdf, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  std::optional<Vector> result =
      bxdf.SampleDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, 0.0, 0.001);
  EXPECT_NEAR(result->y, 0.0, 0.001);
  EXPECT_NEAR(result->z, -1.0, 0.001);
}

TEST(TestMicrofacetBtdf, PdfBTDF) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(TestMicrofacetBtdf, PdfNoIncomingZ) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(TestMicrofacetBtdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                            Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(TestMicrofacetBtdf, PdfSameBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(0.0, bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                 Normalize(Vector(0.0, 1.0, 0.0)),
                                 Bxdf::Hemisphere::BTDF));
}

TEST(TestMicrofacetBtdf, PdfNoHalfAngle) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 1.0);

  EXPECT_EQ(0.0,
            bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                            Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(TestMicrofacetBtdf, Pdf) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_NEAR(2.0,
              bxdf.PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                              Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF),
              0.0001);
}

TEST(TestMicrofacetBtdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBtdf, ReflectanceNoZIncoming) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBtdf, ReflectanceNoZOutgoing) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBtdf, ReflectanceSameBxdfHemispheres) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBtdf, ReflectanceNoHalfAngle) {
  MockReflector reflector;
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 1.0);

  EXPECT_EQ(nullptr, bxdf.ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(TestMicrofacetBtdf, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  TestMicrofacetBtdf bxdf(reflector, kDistribution, 1.0, 2.0);

  const Reflector* reflectance =
      bxdf.ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                              Bxdf::Hemisphere::BTDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.148148, reflectance->Reflectance(1.0), 0.001);
}

}  // namespace
}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
