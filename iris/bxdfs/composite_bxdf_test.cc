#include "iris/bxdfs/composite_bxdf.h"

#define _USE_MATH_CONSTANTS
#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"

TEST(CompositeBxdfTest, IsNotDiffuse) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  EXPECT_FALSE(composite.IsDiffuse(nullptr));

  iris::visual_t diffuse_pdf;
  EXPECT_FALSE(composite.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.0, diffuse_pdf);
}

TEST(CompositeBxdfTest, IsDiffuse) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  EXPECT_TRUE(composite.IsDiffuse(nullptr));

  iris::visual_t diffuse_pdf;
  EXPECT_TRUE(composite.IsDiffuse(&diffuse_pdf));
  EXPECT_EQ(0.5, diffuse_pdf);
}

TEST(CompositeBxdfTest, SampleDiffuseNone) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  auto sample = composite.SampleDiffuse(iris::Vector(1.0, 0.0, 0.0),
                                        iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_FALSE(sample);
}

TEST(CompositeBxdfTest, SampleDiffuse) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.55));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf2;
  EXPECT_CALL(bxdf2, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf2, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf2, SampleDiffuse(iris::Vector(1.0, 0.0, 0.0),
                                   iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(iris::Vector(1.0, 0.0, 0.0)));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1, &bxdf2};
  iris::bxdfs::internal::CompositeBxdf<3> composite(bxdfs, 3u);
  auto sample = composite.SampleDiffuse(iris::Vector(1.0, 0.0, 0.0),
                                        iris::Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(sample);
  EXPECT_EQ(iris::Vector(1.0, 0.0, 0.0), *sample);
}

TEST(CompositeBxdfTest, SampleAllDifuse) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf1, Sample(iris::Vector(1.0, 0.0, 0.0), testing::_,
                            iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(1.0, 0.0, 0.0), std::nullopt, nullptr, 1.0}));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  auto sample = composite.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                                 iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(1.0, 0.0, 0.0), sample->direction);
  EXPECT_FALSE(sample->differentials);
  EXPECT_EQ(nullptr, sample->bxdf_override);
  EXPECT_EQ(1.0, sample->pdf_weight);
}

TEST(CompositeBxdfTest, SampleOneDiffuse) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.2));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf2;
  EXPECT_CALL(bxdf2, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf2, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf3;
  EXPECT_CALL(bxdf3, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf3, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf3, Sample(iris::Vector(1.0, 0.0, 0.0), testing::_,
                            iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(1.0, 0.0, 0.0), std::nullopt, nullptr, 1.0}));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1, &bxdf2, &bxdf3};
  iris::bxdfs::internal::CompositeBxdf<4> composite(bxdfs, 4u);
  auto sample = composite.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                                 iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(1.0, 0.0, 0.0), sample->direction);
  EXPECT_FALSE(sample->differentials);
  EXPECT_EQ(nullptr, sample->bxdf_override);
  EXPECT_EQ(0.25, sample->pdf_weight);
}

TEST(CompositeBxdfTest, SampleSpecular) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
  EXPECT_CALL(rng, DiscardGeometric(1));
  iris::Sampler sampler(rng);

  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  EXPECT_CALL(bxdf1, Sample(iris::Vector(1.0, 0.0, 0.0), testing::_,
                            iris::Vector(0.0, 0.0, 1.0), testing::_))
      .WillOnce(testing::Return(iris::Bxdf::SampleResult{
          iris::Vector(1.0, 0.0, 0.0), std::nullopt, &bxdf1, 1.0}));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  auto sample = composite.Sample(iris::Vector(1.0, 0.0, 0.0), std::nullopt,
                                 iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(1.0, 0.0, 0.0), sample->direction);
  EXPECT_FALSE(sample->differentials);
  EXPECT_EQ(&bxdf1, sample->bxdf_override);
  EXPECT_EQ(0.5, sample->pdf_weight);
}

TEST(CompositeBxdfTest, PdfAllSpecular) {
  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  const iris::Bxdf* bxdfs[] = {&bxdf, &bxdf};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  EXPECT_EQ(
      0.0,
      composite.Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfOneBxdf) {
  iris::bxdfs::MockBxdf bxdf;
  EXPECT_CALL(bxdf, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(1.0));

  const iris::Bxdf* bxdfs[] = {&bxdf};
  iris::bxdfs::internal::CompositeBxdf<1> composite(bxdfs, 2u);
  EXPECT_EQ(
      1.0,
      composite.Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfTwoBxdfs) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf0,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(1.0));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(2.0)));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  EXPECT_EQ(
      1.5,
      composite.Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, PdfTwoBxdfsOneSpecular) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(0.0),
                                     testing::Return(false)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(false));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(3.0)));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  EXPECT_EQ(
      3.0,
      composite.Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                    iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF));
}

TEST(CompositeBxdfTest, Reflectance) {
  iris::reflectors::MockReflector reflector0;
  EXPECT_CALL(reflector0, Reflectance(1.0)).WillOnce(testing::Return(0.25));
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf0, Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(-1.0, 0.0, 0.0),
                                 iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector0));
  iris::reflectors::MockReflector reflector1;
  EXPECT_CALL(reflector1, Reflectance(1.0)).WillOnce(testing::Return(0.5));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf1, Reflectance(iris::Vector(1.0, 0.0, 0.0),
                                 iris::Vector(-1.0, 0.0, 0.0),
                                 iris::Bxdf::Hemisphere::BRDF, testing::_))
      .WillOnce(testing::Return(&reflector1));

  const iris::Bxdf* bxdfs[] = {&bxdf0, &bxdf1};
  iris::bxdfs::internal::CompositeBxdf<2> composite(bxdfs, 2u);
  auto* result = composite.Reflectance(
      iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_EQ(0.75, result->Reflectance(1.0));
}

TEST(CompositeBxdfTest, MakeComposite) {
  iris::bxdfs::MockBxdf bxdf0;
  EXPECT_CALL(bxdf0, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf0, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf0,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(1.0));

  iris::bxdfs::MockBxdf bxdf1;
  EXPECT_CALL(bxdf1, IsDiffuse(testing::NotNull()))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<0>(1.0),
                                     testing::Return(true)));
  EXPECT_CALL(bxdf1, IsDiffuse(testing::IsNull()))
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(bxdf1,
              Pdf(iris::Vector(1.0, 0.0, 0.0), iris::Vector(-1.0, 0.0, 0.0),
                  iris::Vector(0.0, 0.0, 1.0), iris::Bxdf::Hemisphere::BRDF))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(2.0)));

  const iris::Bxdf* composite =
      iris::bxdfs::MakeComposite(iris::testing::GetBxdfAllocator(), &bxdf0,
                                 &bxdf1, nullptr, nullptr, nullptr);
  ASSERT_TRUE(composite);

  EXPECT_EQ(1.5, composite->Pdf(iris::Vector(1.0, 0.0, 0.0),
                                iris::Vector(-1.0, 0.0, 0.0),
                                iris::Vector(0.0, 0.0, 1.0),
                                iris::Bxdf::Hemisphere::BRDF));
}