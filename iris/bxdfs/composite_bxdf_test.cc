#include "iris/bxdfs/composite_bxdf.h"

#define _USE_MATH_CONSTANTS
#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"

TEST(CompositeBxdfTest, Sample) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::MockBxdf bxdf0;
  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1,
              Sample(iris::Vector(1.0, 0.0, 0.0), testing::_, testing::_))
      .WillOnce(testing::Return(
          iris::Bxdf::SampleResult{iris::Vector(1.0, 0.0, 0.0)}));

  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdf0, bxdf1);
  auto sample =
      composite.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt, sampler);
  EXPECT_EQ(iris::Vector(1.0, 0.0, 0.0), sample->direction);
  EXPECT_FALSE(sample->differentials);
  EXPECT_EQ(nullptr, sample->sample_source);
}

TEST(CompositeBxdfTest, PdfOneBxdf) {
  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(std::nullopt));

  iris::bxdfs::internal::CompositeBxdf<1> composite(bxdf);
  EXPECT_EQ(std::nullopt, composite.Pdf(iris::Vector(1.0, 0.0, 0.0),
                                        iris::Vector(-1.0, 0.0, 0.0), nullptr,
                                        iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfTwoBxdfsEmpty) {
  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillRepeatedly(testing::Return(std::nullopt));

  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdf, bxdf);
  EXPECT_EQ(std::nullopt, composite.Pdf(iris::Vector(1.0, 0.0, 0.0),
                                        iris::Vector(-1.0, 0.0, 0.0), nullptr,
                                        iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfTwoBxdfs) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(std::nullopt));
  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(2.0));

  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdf0, bxdf1);
  EXPECT_EQ(1.5, composite.Pdf(iris::Vector(1.0, 0.0, 0.0),
                               iris::Vector(-1.0, 0.0, 0.0), nullptr,
                               iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, LightPdfTwoBxdfs) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(0.0));
  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(2.0));

  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdf0, bxdf1);
  EXPECT_EQ(1.0, composite.Pdf(iris::Vector(1.0, 0.0, 0.0),
                               iris::Vector(-1.0, 0.0, 0.0), nullptr,
                               iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, LightPdfTwoBxdfsMissingOne) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(1.0));
  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(std::nullopt));

  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdf0, bxdf1);
  EXPECT_EQ(1.0, composite.Pdf(iris::Vector(1.0, 0.0, 0.0),
                               iris::Vector(-1.0, 0.0, 0.0), nullptr,
                               iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, LightPdfTwoBxdfsMissingBoth) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(std::nullopt));
  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(std::nullopt));

  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdf0, bxdf1);
  EXPECT_FALSE(composite.Pdf(iris::Vector(1.0, 0.0, 0.0),
                             iris::Vector(-1.0, 0.0, 0.0), nullptr,
                             iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, Reflectance) {
  iris::reflectors::MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(testing::Return(0.25));
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(-1.0, 0.0, 0.0), nullptr,
                                 iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector0));
  iris::reflectors::MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(testing::Return(0.5));
  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(-1.0, 0.0, 0.0), nullptr,
                                 iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector1));

  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdf0, bxdf1);
  auto* result = composite.Reflectance(
      iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_EQ(0.75, result->Reflectance(1.0));
}

TEST(CompositeBxdfTest, MakeComposite) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(std::nullopt));
  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  nullptr, iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(2.0));

  const auto& composite = iris::bxdfs::MakeComposite(
      iris::testing::GetBxdfAllocator(), bxdf0, bxdf1);
  EXPECT_EQ(1.5, composite.Pdf(iris::Vector(1.0, 0.0, 0.0),
                               iris::Vector(-1.0, 0.0, 0.0), nullptr,
                               iris::Bxdf::Hemisphere::BRDF));
}