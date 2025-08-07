#include "iris/bxdfs/microfacet_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::random::MockRandom;
using ::iris::reflectors::MockReflector;
using ::iris::spectra::MockSpectrum;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::testing::_;
using ::testing::Return;

TEST(MicrofacetDielectricBrdf, Nullptr) {
  MockReflector reflector;
  EXPECT_TRUE(MakeMicrofacetDielectricBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                           1.5, 1.0, 1.0, true));
  EXPECT_FALSE(MakeMicrofacetDielectricBrdf(GetBxdfAllocator(), nullptr, 1.0,
                                            1.5, 1.0, 1.0, true));
  EXPECT_FALSE(MakeMicrofacetDielectricBrdf(GetBxdfAllocator(), &reflector,
                                            -1.0, 1.5, 1.0, 1.0, true));
  EXPECT_FALSE(MakeMicrofacetDielectricBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                            -1.0, 1.0, 1.0, true));
}

TEST(MicrofacetDielectricBrdf, SampleDiffuseZero) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetDielectricBrdf, SampleDiffuseOppositeBxdfHemispheres) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(1.0));
  Sampler sampler(rng);

  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetDielectricBrdf, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 0.1, 0.1, true);

  std::optional<Vector> sample = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_NEAR(sample->x, 0.0, 0.001);
  EXPECT_NEAR(sample->y, 0.97555, 0.001);
  EXPECT_NEAR(sample->z, 0.21975, 0.001);
}

TEST(MicrofacetDielectricBrdf, PdfBTDF) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetDielectricBrdf, PdfNoIncomingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetDielectricBrdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetDielectricBrdf, PdfDifferentBxdfHemispheres) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(0.0, bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                                  Normalize(Vector(0.0, 1.0, 0.0)),
                                  Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetDielectricBrdf, Pdf) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_NEAR(0.03026,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.0001);
}

TEST(MicrofacetDielectricBrdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBrdf, ReflectanceNoZIncoming) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBrdf, ReflectanceNoZOutgoing) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBrdf, ReflectanceOppositeBxdfHemispheres) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBrdf, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  const Bxdf* bxdf = MakeMicrofacetDielectricBrdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.5, 1.0, 1.0, true);

  const Reflector* reflectance =
      bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.000605, reflectance->Reflectance(1.0), 0.00001);
}

TEST(MicrofacetConductorBrdf, Nullptr) {
  MockReflector reflector;
  MockSpectrum conductor;
  EXPECT_TRUE(MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                          &conductor, &conductor, 1.0, 1.0,
                                          true));
  EXPECT_TRUE(MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                          nullptr, nullptr, 1.0, 1.0, true));
  EXPECT_FALSE(MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 0.0,
                                           &conductor, &conductor, 1.0, 1.0,
                                           true));
}
TEST(MicrofacetConductorBrdf, SampleDiffuseZero) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetConductorBrdf, SampleDiffuseOppositeBxdfHemispheres) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetConductorBrdf, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.1, 0.1, true);

  std::optional<Vector> sample = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_NEAR(sample->x, 0.0, 0.001);
  EXPECT_NEAR(sample->y, 0.97555, 0.001);
  EXPECT_NEAR(sample->z, 0.21975, 0.001);
}

TEST(MicrofacetConductorBrdf, PdfBTDF) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetConductorBrdf, PdfNoIncomingZ) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetConductorBrdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetConductorBrdf, PdfDifferentBxdfHemispheres) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(0.0, bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                                  Normalize(Vector(0.0, 1.0, 0.0)),
                                  Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetConductorBrdf, Pdf) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_NEAR(0.06222,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.0001);
}

TEST(MicrofacetConductorBrdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(MicrofacetConductorBrdf, ReflectanceNoZIncoming) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(MicrofacetConductorBrdf, ReflectanceNoZOutgoing) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(MicrofacetConductorBrdf, ReflectanceOppositeBxdfHemispheres) {
  MockReflector reflector;
  MockSpectrum conductor_eta;
  MockSpectrum conductor_k;
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, &conductor_k, 0.5, 0.5, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(MicrofacetConductorBrdf, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  MockSpectrum conductor_eta;
  EXPECT_CALL(conductor_eta, Intensity(_)).WillOnce(Return(0.5));
  const Bxdf* bxdf =
      MakeMicrofacetConductorBrdf(GetBxdfAllocator(), &reflector, 1.0,
                                  &conductor_eta, nullptr, 0.5, 0.5, true);

  const Reflector* reflectance =
      bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                               Bxdf::Hemisphere::BRDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.003457, reflectance->Reflectance(1.0), 0.001);
}

TEST(MicrofacetDielectricBtdf, Nullptr) {
  MockReflector reflector;
  EXPECT_TRUE(MakeMicrofacetDielectricBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                           2.0, 1.0, 1.0, true));
  EXPECT_FALSE(MakeMicrofacetDielectricBtdf(GetBxdfAllocator(), nullptr, 1.0,
                                            2.0, 1.0, 1.0, true));
  EXPECT_FALSE(MakeMicrofacetDielectricBtdf(GetBxdfAllocator(), &reflector, 0.0,
                                            2.0, 1.0, 1.0, true));
  EXPECT_FALSE(MakeMicrofacetDielectricBtdf(GetBxdfAllocator(), &reflector, 1.0,
                                            0.0, 1.0, 1.0, true));
}

TEST(MicrofacetDielectricBtdf, SampleDiffuseZero) {
  MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetDielectricBtdf, SampleDiffuseSameBxdfHemispheres) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(1.0));
  Sampler sampler(rng);

  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.0, 1.0, 1.0, true);

  EXPECT_FALSE(bxdf->SampleDiffuse(Vector(1.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                   sampler));
}

TEST(MicrofacetDielectricBtdf, SampleDiffuse) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.75));
  Sampler sampler(rng);

  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  std::optional<Vector> sample = bxdf->SampleDiffuse(
      Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_NEAR(sample->x, 0.0, 0.001);
  EXPECT_NEAR(sample->y, -0.673002, 0.001);
  EXPECT_NEAR(sample->z, -0.739640, 0.001);
}

TEST(MicrofacetDielectricBtdf, PdfBTDF) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(MicrofacetDielectricBtdf, PdfNoIncomingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetDielectricBtdf, PdfNoOutgoingZ) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 0.0),
                             Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetDielectricBtdf, PdfSameBxdfHemispheres) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(0.0, bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                  Normalize(Vector(0.0, 1.0, 0.0)),
                                  Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetDielectricBtdf, PdfNoHalfAngle) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.0, 1.0, 1.0, true);

  EXPECT_EQ(0.0,
            bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                             Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(MicrofacetDielectricBtdf, Pdf) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_NEAR(0.48430,
              bxdf->PdfDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                               Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF),
              0.0001);
}

TEST(MicrofacetDielectricBtdf, ReflectanceWrongHemishphere) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BRDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBtdf, ReflectanceNoZIncoming) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBtdf, ReflectanceNoZOutgoing) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBtdf, ReflectanceSameBxdfHemispheres) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBtdf, ReflectanceNoHalfAngle) {
  MockReflector reflector;
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 1.0, 1.0, 1.0, true);

  EXPECT_EQ(nullptr, bxdf->ReflectanceDiffuse(
                         Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                         Bxdf::Hemisphere::BTDF, GetSpectralAllocator()));
}

TEST(MicrofacetDielectricBtdf, Reflectance) {
  MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(0.5));
  const Bxdf* bxdf = MakeMicrofacetDielectricBtdf(
      GetBxdfAllocator(), &reflector, 1.0, 2.0, 1.0, 1.0, true);

  const Reflector* reflectance =
      bxdf->ReflectanceDiffuse(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                               Bxdf::Hemisphere::BTDF, GetSpectralAllocator());
  ASSERT_TRUE(reflectance);
  EXPECT_NEAR(0.05381, reflectance->Reflectance(1.0), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris
