#include "iris/bxdfs/specular_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/testing/spectral_allocator.h"

TEST(SpecularBrdfTest, IsDiffuse) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  EXPECT_FALSE(bxdf.IsDiffuse());
}

TEST(SpecularBrdfTest, SampleDiffuse) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto result = bxdf.SampleDiffuse(iris::Vector(1.0, 1.0, 1.0),
                                   iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(SpecularBrdfTest, Sample) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto result = bxdf.Sample(iris::Vector(1.0, 1.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBrdfTest, SampleWithDerivatives) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto result =
      bxdf.Sample(iris::Vector(1.0, 1.0, 1.0),
                  {{iris::Vector(1.0, 0.5, 1.0), iris::Vector(0.5, 1.0, 1.0)}},
                  iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(iris::Vector(-1.0, -0.5, 1.0), result->differentials->dx);
  EXPECT_EQ(iris::Vector(-0.5, -1.0, 1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBrdfTest, PdfBtdfSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(
      0.0,
      bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
               iris::Vector(0.0, 0.0, -1.0), iris::Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBrdfTest, PdfReflected) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(
      1.0, bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF));
}

TEST(SpecularBrdfTest, ReflectanceBtdf) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBrdfTest, Reflectance) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBtdfTest, IsDiffuse) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  EXPECT_FALSE(bxdf.IsDiffuse());
}

TEST(SpecularBtdfTest, SampleDiffuse) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto result = bxdf.SampleDiffuse(iris::Vector(1.0, 1.0, 1.0),
                                   iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(SpecularBtdfTest, SampleFront) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto result = bxdf.Sample(iris::Vector(1.0, 1.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBtdfTest, SampleBack) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto result = bxdf.Sample(iris::Vector(1.0, 1.0, -1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBtdfTest, SampleWithDerivatives) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto result =
      bxdf.Sample(iris::Vector(1.0, 1.0, 1.0),
                  {{iris::Vector(1.0, 0.5, 1.0), iris::Vector(0.5, 1.0, 1.0)}},
                  iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, -1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(iris::Vector(-1.0, -0.5, -1.0), result->differentials->dx);
  EXPECT_EQ(iris::Vector(-0.5, -1.0, -1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBtdfTest, PdfBrdfSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(
      0.0, bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF));
}

TEST(SpecularBtdfTest, PdfTransmitted) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(
      1.0,
      bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
               iris::Vector(0.0, 0.0, -1.0), iris::Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBtdfTest, ReflectanceBrdf) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBtdfTest, ReflectanceFront) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBtdfTest, ReflectanceBack) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBxdfTest, SampleDiffuse) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result = bxdf.SampleDiffuse(iris::Vector(1.0, 1.0, 1.0),
                                   iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(result);
}

TEST(SpecularBxdfTest, SampleTransmittanceFront) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result = bxdf.Sample(iris::Vector(1.0, 1.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, -1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleTransmittanceBack) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result = bxdf.Sample(iris::Vector(1.0, 1.0, -1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleTransmittanceWithDerivatives) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(1.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto result =
      bxdf.Sample(iris::Vector(1.0, 1.0, 1.0),
                  {{iris::Vector(1.0, 0.5, 1.0), iris::Vector(0.5, 1.0, 1.0)}},
                  iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, -1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(iris::Vector(-1.0, -0.5, -1.0), result->differentials->dx);
  EXPECT_EQ(iris::Vector(-0.5, -1.0, -1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleReflectance) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto result = bxdf.Sample(iris::Vector(1.0, 1.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, SampleReflectanceWithDerivatives) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.0));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto result =
      bxdf.Sample(iris::Vector(1.0, 1.0, 1.0),
                  {{iris::Vector(1.0, 0.5, 1.0), iris::Vector(0.5, 1.0, 1.0)}},
                  iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(iris::Vector(-1.0, -0.5, 1.0), result->differentials->dx);
  EXPECT_EQ(iris::Vector(-0.5, -1.0, 1.0), result->differentials->dy);
  EXPECT_EQ(result->pdf_weight, 1.0);
}

TEST(SpecularBxdfTest, PdfWrongHemisphere) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_EQ(
      0.0, bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF));
  EXPECT_EQ(
      0.0, bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBxdfTest, PdfFront) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_NEAR(
      0.0400016233,
      bxdf.Pdf(iris::Normalize(iris::Vector(0.1, 0.0, 1.0)),
               iris::Normalize(iris::Vector(-0.1, 0.0, 1.0)),
               iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF),
      0.001);
  EXPECT_NEAR(
      0.959998369,
      bxdf.Pdf(iris::Normalize(iris::Vector(0.1, 0.0, 1.0)),
               iris::Normalize(iris::Vector(0.1, 0.0, -1.0)),
               iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BTDF),
      0.001);
}

TEST(SpecularBxdfTest, PdfBack) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_NEAR(
      0.0400016233,
      bxdf.Pdf(iris::Normalize(iris::Vector(0.1, 0.0, -1.0)),
               iris::Normalize(iris::Vector(-0.1, 0.0, -1.0)),
               iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF),
      0.001);
  EXPECT_NEAR(
      0.959998369,
      bxdf.Pdf(iris::Normalize(iris::Vector(0.1, 0.0, -1.0)),
               iris::Normalize(iris::Vector(0.1, 0.0, 1.0)),
               iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BTDF),
      0.001);
}

TEST(SpecularBxdfTest, ReflectanceWrongHemisphere) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  EXPECT_FALSE(bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator()));
  EXPECT_FALSE(bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator()));
}

TEST(SpecularBxdfTest, TransmittanceFront) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::UniformReflector transmitter(1.0);
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto* result = bxdf.Reflectance(iris::Normalize(iris::Vector(0.1, 0.0, 1.0)),
                                  iris::Normalize(iris::Vector(0.1, 0.0, -1.0)),
                                  iris::Bxdf::Hemisphere::BTDF,
                                  iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.426665962, result->Reflectance(1.0), 0.0001);
}

TEST(SpecularBxdfTest, TransmittanceBack) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::UniformReflector transmitter(1.0);
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto* result = bxdf.Reflectance(iris::Normalize(iris::Vector(0.1, 0.0, -1.0)),
                                  iris::Normalize(iris::Vector(0.1, 0.0, 1.0)),
                                  iris::Bxdf::Hemisphere::BTDF,
                                  iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(2.15998125, result->Reflectance(1.0), 0.0001);
}

TEST(SpecularBxdfTest, Reflectance) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.5);
  auto* result = bxdf.Reflectance(iris::Normalize(iris::Vector(0.1, 0.0, 1.0)),
                                  iris::Normalize(iris::Vector(-0.1, 0.0, 1.0)),
                                  iris::Bxdf::Hemisphere::BRDF,
                                  iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.0400016233, result->Reflectance(1.0), 0.001);
}