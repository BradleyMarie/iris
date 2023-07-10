#include "iris/bxdfs/mirror_brdf.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

TEST(MirrorBrdfTest, Sample) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::MirrorBrdf bxdf(reflector);
  auto result = bxdf.Sample(iris::Vector(1.0, 1.0, 1.0), std::nullopt, sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
}

TEST(MirrorBrdfTest, SampleWithDerivatives) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, DiscardGeometric(2));
  iris::Sampler sampler(rng);

  iris::bxdfs::MirrorBrdf bxdf(reflector);
  auto result = bxdf.Sample(
      iris::Vector(1.0, 1.0, 1.0),
      {{iris::Vector(1.0, 0.5, 1.0), iris::Vector(0.5, 1.0, 1.0)}}, sampler);
  EXPECT_EQ(iris::Vector(-1.0, -1.0, 1.0), result->direction);
  ASSERT_TRUE(result->differentials);
  EXPECT_EQ(iris::Vector(-1.0, -0.5, 1.0), result->differentials->dx);
  EXPECT_EQ(iris::Vector(-0.5, -1.0, 1.0), result->differentials->dy);
}

TEST(MirrorBrdfTest, PdfBtdfSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::MirrorBrdf bxdf(reflector);
  EXPECT_EQ(0.0,
            (bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                      &bxdf, iris::Bxdf::Hemisphere::BTDF)));
}

TEST(MirrorBrdfTest, PdfWrongSource) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::MirrorBrdf bxdf(reflector);
  EXPECT_EQ(0.0,
            (bxdf.Pdf(iris::Vector(0.0, 1.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                      nullptr, iris::Bxdf::Hemisphere::BRDF)));
}

TEST(MirrorBrdfTest, PdfReflected) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::MirrorBrdf bxdf(reflector);
  EXPECT_FALSE(bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0),
                        iris::Vector(0.0, 0.0, 1.0), &bxdf,
                        iris::Bxdf::Hemisphere::BRDF));
}

TEST(MirrorBrdfTest, ReflectanceBtdf) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::MirrorBrdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(MirrorBrdfTest, ReflectanceWrongSourceSampled) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::MirrorBrdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), nullptr,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_FALSE(result);
}

TEST(MirrorBrdfTest, Reflectance) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::MirrorBrdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  EXPECT_EQ(&reflector, result);
}