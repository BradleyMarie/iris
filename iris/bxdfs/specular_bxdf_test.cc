#include "iris/bxdfs/specular_bxdf.h"

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

TEST(SpecularBrdfTest, IsDiffuse) {
  reflectors::MockReflector reflector;
  SpecularBrdf bxdf(reflector, FresnelNoOp());
  EXPECT_FALSE(bxdf.IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_FALSE(bxdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.0, diffuse_pdf);
}

TEST(SpecularBrdfTest, SampleDiffuse) {
  reflectors::MockReflector reflector;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBrdf bxdf(reflector, FresnelNoOp());
  auto result =
      bxdf.SampleDiffuse(Vector(1.0, 1.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(SpecularBrdfTest, Sample) {
  reflectors::MockReflector reflector;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBrdf bxdf(reflector, FresnelNoOp());
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBrdfTest, SampleWithDerivatives) {
  reflectors::MockReflector reflector;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBrdf bxdf(reflector, FresnelNoOp());
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0),
                            {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, 1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, 1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBrdfTest, PdfBtdfSampled) {
  reflectors::MockReflector reflector;
  SpecularBrdf bxdf(reflector, FresnelNoOp());
  EXPECT_EQ(0.0, bxdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBrdfTest, PdfReflected) {
  reflectors::MockReflector reflector;
  SpecularBrdf bxdf(reflector, FresnelNoOp());
  EXPECT_EQ(1.0, bxdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(SpecularBrdfTest, ReflectanceBtdf) {
  reflectors::MockReflector reflector;
  SpecularBrdf bxdf(reflector, FresnelNoOp());
  auto* result =
      bxdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BTDF, testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBrdfTest, Reflectance) {
  reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(1.0));
  SpecularBrdf bxdf(reflector, FresnelNoOp());
  auto* result =
      bxdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BRDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBtdfTest, IsDiffuse) {
  reflectors::MockReflector reflector;
  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());

  visual_t diffuse_pdf;
  EXPECT_FALSE(bxdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.0, diffuse_pdf);
}

TEST(SpecularBtdfTest, SampleDiffuse) {
  reflectors::MockReflector reflector;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  auto result =
      bxdf.SampleDiffuse(Vector(1.0, 1.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(SpecularBtdfTest, SampleFront) {
  reflectors::MockReflector reflector;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBtdfTest, SampleBack) {
  reflectors::MockReflector reflector;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  auto result = bxdf.Sample(Vector(1.0, 1.0, -1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBtdfTest, SampleWithDerivatives) {
  reflectors::MockReflector reflector;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0),
                            {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, -1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, -1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBtdfTest, PdfBrdfSampled) {
  reflectors::MockReflector reflector;
  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  EXPECT_EQ(0.0, bxdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
}

TEST(SpecularBtdfTest, PdfTransmitted) {
  reflectors::MockReflector reflector;
  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  EXPECT_EQ(1.0, bxdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Vector(0.0, 0.0, -1.0), Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBtdfTest, ReflectanceBrdf) {
  reflectors::MockReflector reflector;
  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  auto* result =
      bxdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                       Bxdf::Hemisphere::BRDF, testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBtdfTest, ReflectanceFront) {
  reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(1.0));
  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  auto* result =
      bxdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                       Bxdf::Hemisphere::BTDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBtdfTest, ReflectanceBack) {
  reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(1.0));
  SpecularBtdf bxdf(reflector, 1.0, 1.0, FresnelNoOp());
  auto* result =
      bxdf.Reflectance(Vector(0.0, 0.0, -1.0), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BTDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBxdfTest, IsDiffuse) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  EXPECT_FALSE(bxdf.IsDiffuse(nullptr));

  visual_t diffuse_pdf;
  EXPECT_FALSE(bxdf.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.0, diffuse_pdf);
}

TEST(SpecularBxdfTest, SampleDiffuse) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result =
      bxdf.SampleDiffuse(Vector(1.0, 1.0, 1.0), Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(SpecularBxdfTest, SampleTransmittanceFront) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleTransmittanceBack) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result = bxdf.Sample(Vector(1.0, 1.0, -1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleTransmittanceWithDerivatives) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0),
                            {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, -1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, -1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, -1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleReflectance) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0), std::nullopt,
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleReflectanceWithDerivatives) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  Sampler sampler(rng);

  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto result = bxdf.Sample(Vector(1.0, 1.0, 1.0),
                            {{Vector(1.0, 0.5, 1.0), Vector(0.5, 1.0, 1.0)}},
                            Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(Vector(-1.0, -1.0, 1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(Vector(-1.0, -0.5, 1.0), result->differentials->dx);
  EXPECT_EQ(Vector(-0.5, -1.0, 1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, PdfWrongHemisphere) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_EQ(0.0, bxdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                          Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF));
  EXPECT_EQ(0.0, bxdf.Pdf(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                          Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBxdfTest, PdfFront) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_NEAR(0.0400016233,
              bxdf.Pdf(Normalize(Vector(0.1, 0.0, 1.0)),
                       Normalize(Vector(-0.1, 0.0, 1.0)), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BRDF),
              0.001);
  EXPECT_NEAR(0.959998369,
              bxdf.Pdf(Normalize(Vector(0.1, 0.0, 1.0)),
                       Normalize(Vector(0.1, 0.0, -1.0)), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BTDF),
              0.001);
}

TEST(SpecularBxdfTest, PdfBack) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_NEAR(0.0400016233,
              bxdf.Pdf(Normalize(Vector(0.1, 0.0, -1.0)),
                       Normalize(Vector(-0.1, 0.0, -1.0)),
                       Vector(0.0, 0.0, 1.0), Bxdf::Hemisphere::BRDF),
              0.001);
  EXPECT_NEAR(0.959998369,
              bxdf.Pdf(Normalize(Vector(0.1, 0.0, -1.0)),
                       Normalize(Vector(0.1, 0.0, 1.0)), Vector(0.0, 0.0, 1.0),
                       Bxdf::Hemisphere::BTDF),
              0.001);
}

TEST(SpecularBxdfTest, ReflectanceWrongHemisphere) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_FALSE(bxdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0),
                                Bxdf::Hemisphere::BRDF,
                                testing::GetSpectralAllocator()));
  EXPECT_FALSE(bxdf.Reflectance(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, 1.0),
                                Bxdf::Hemisphere::BTDF,
                                testing::GetSpectralAllocator()));
}

TEST(SpecularBxdfTest, TransmittanceFront) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  EXPECT_CALL(transmitter, Reflectance(_)).WillOnce(Return(1.0));
  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto* result = bxdf.Reflectance(
      Normalize(Vector(0.1, 0.0, 1.0)), Normalize(Vector(0.1, 0.0, -1.0)),
      Bxdf::Hemisphere::BTDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.426665962, result->Reflectance(1.0), 0.0001);
}

TEST(SpecularBxdfTest, TransmittanceBack) {
  reflectors::MockReflector reflector;
  reflectors::MockReflector transmitter;
  EXPECT_CALL(transmitter, Reflectance(_)).WillOnce(Return(1.0));
  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto* result = bxdf.Reflectance(
      Normalize(Vector(0.1, 0.0, -1.0)), Normalize(Vector(0.1, 0.0, 1.0)),
      Bxdf::Hemisphere::BTDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(2.15998125, result->Reflectance(1.0), 0.0001);
}

TEST(SpecularBxdfTest, Reflectance) {
  reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(_)).WillOnce(Return(1.0));
  reflectors::MockReflector transmitter;
  SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto* result = bxdf.Reflectance(
      Normalize(Vector(0.1, 0.0, 1.0)), Normalize(Vector(-0.1, 0.0, 1.0)),
      Bxdf::Hemisphere::BRDF, testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.0400016233, result->Reflectance(1.0), 0.001);
}

}  // namespace
}  // namespace bxdfs
}  // namespace iris