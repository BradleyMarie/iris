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
}

TEST(SpecularBrdfTest, PdfBtdfSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(0.0,
            (bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                      iris::Vector(0.0, 0.0, -1.0), &bxdf,
                      iris::Bxdf::Hemisphere::BTDF)));
}

TEST(SpecularBrdfTest, PdfWrongSource) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(0.0,
            (bxdf.Pdf(iris::Vector(0.0, 1.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                      iris::Vector(0.0, 0.0, 1.0), nullptr,
                      iris::Bxdf::Hemisphere::BRDF)));
}

TEST(SpecularBrdfTest, PdfReflected) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  EXPECT_FALSE(bxdf.Pdf(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Vector(0.0, 0.0, 1.0), &bxdf, iris::Bxdf::Hemisphere::BRDF));
}

TEST(SpecularBrdfTest, ReflectanceBtdf) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBrdfTest, ReflectanceWrongSourceSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBrdfTest, Reflectance) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::bxdfs::SpecularBrdf bxdf(reflector, iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
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
}

TEST(SpecularBtdfTest, PdfBrdfSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(0.0,
            (bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                      iris::Vector(0.0, 0.0, 1.0), &bxdf,
                      iris::Bxdf::Hemisphere::BRDF)));
}

TEST(SpecularBtdfTest, PdfWrongSource) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  EXPECT_EQ(0.0,
            (bxdf.Pdf(iris::Vector(0.0, 1.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                      iris::Vector(0.0, 0.0, -1.0), nullptr,
                      iris::Bxdf::Hemisphere::BTDF)));
}

TEST(SpecularBtdfTest, PdfTransmitted) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  EXPECT_FALSE(bxdf.Pdf(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Vector(0.0, 0.0, -1.0), &bxdf, iris::Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBtdfTest, ReflectanceBrdf) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBtdfTest, ReflectanceWrongSourceSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), nullptr,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBtdfTest, ReflectanceFront) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBtdfTest, ReflectanceBack) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::bxdfs::SpecularBtdf bxdf(reflector, 1.0, 1.0,
                                 iris::bxdfs::FresnelNoOp());
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
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
}

TEST(SpecularBxdfTest, PdfWrongHemisphere) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  EXPECT_EQ(0.0,
            bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
                     iris::Vector(0.0, 0.0, 1.0), &bxdf,
                     iris::Bxdf::Hemisphere::BRDF));
  EXPECT_EQ(0.0,
            bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                     iris::Vector(0.0, 0.0, 1.0), &bxdf,
                     iris::Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBxdfTest, PdfWrongSource) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  EXPECT_EQ(0.0,
            (bxdf.Pdf(iris::Vector(0.0, 1.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                      iris::Vector(0.0, 0.0, -1.0), nullptr,
                      iris::Bxdf::Hemisphere::BTDF)));
}

TEST(SpecularBxdfTest, PdfFront) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  EXPECT_FALSE(bxdf.Pdf(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Vector(0.0, 0.0, 1.0), &bxdf, iris::Bxdf::Hemisphere::BRDF));
  EXPECT_FALSE(bxdf.Pdf(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
      iris::Vector(0.0, 0.0, 1.0), &bxdf, iris::Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBxdfTest, PdfBack) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  EXPECT_FALSE(bxdf.Pdf(
      iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, -1.0),
      iris::Vector(0.0, 0.0, 1.0), &bxdf, iris::Bxdf::Hemisphere::BRDF));
  EXPECT_FALSE(bxdf.Pdf(
      iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, 1.0),
      iris::Vector(0.0, 0.0, 1.0), &bxdf, iris::Bxdf::Hemisphere::BTDF));
}

TEST(SpecularBxdfTest, ReflectanceWrongHemisphere) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  EXPECT_FALSE(bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator()));
  EXPECT_FALSE(bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator()));
}

TEST(SpecularBxdfTest, ReflectanceWrongSourceSampled) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), nullptr,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(SpecularBxdfTest, TransmittanceFront) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::UniformReflector transmitter(1.0);
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBxdfTest, TransmittanceBack) {
  iris::reflectors::MockReflector reflector;
  iris::reflectors::UniformReflector transmitter(1.0);
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}

TEST(SpecularBxdfTest, Reflectance) {
  iris::reflectors::UniformReflector reflector(1.0);
  iris::reflectors::MockReflector transmitter;
  iris::bxdfs::SpecularBxdf bxdf(reflector, transmitter, 1.0, 1.0);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_EQ(1.0, result->Reflectance(1.0));
}