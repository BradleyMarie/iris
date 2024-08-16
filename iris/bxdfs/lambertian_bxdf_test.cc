#define _USE_MATH_DEFINES
#include "iris/bxdfs/lambertian_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

TEST(LambertianBrdfTest, IsDiffuse) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::LambertianBrdf bxdf(reflector);
  EXPECT_TRUE(bxdf.IsDiffuse());
}

TEST(LambertianBrdfTest, SampleDiffuseAligned) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::LambertianBrdf bxdf(reflector);
  auto result = bxdf.SampleDiffuse(iris::Vector(0.0, 0.0, 1.0),
                                   iris::Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, 0.0003452669, 0.0001);
}

TEST(LambertianBrdfTest, SampleDiffuseUnaligned) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::LambertianBrdf bxdf(reflector);
  auto result = bxdf.SampleDiffuse(iris::Vector(0.0, 0.0, 1.0),
                                   iris::Vector(0.0, 0.0, -1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, 0.707106709, 0.0001);
  EXPECT_NEAR(result->y, 0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(LambertianBrdfTest, Sample) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::LambertianBrdf bxdf(reflector);
  auto result = bxdf.Sample(iris::Vector(0.0, 0.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->direction.x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->direction.y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->direction.z, 0.0003452669, 0.0001);
  EXPECT_FALSE(result->differentials);
}

TEST(LambertianBrdfTest, DiffusePdfTransmitted) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::LambertianBrdf bxdf(reflector);
  EXPECT_EQ(0.0,
            bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
                     iris::Vector(0.0, 0.0, -1.0), &bxdf,
                     iris::Bxdf::Hemisphere::BTDF));
}

TEST(LambertianBrdfTest, DiffusePdfReflected) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::LambertianBrdf bxdf(reflector);
  EXPECT_NEAR(
      M_1_PI,
      *bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                iris::Vector(0.0, 0.0, -1.0), &bxdf,
                iris::Bxdf::Hemisphere::BRDF),
      0.001);
}

TEST(LambertianBrdfTest, ReflectanceBtdf) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::LambertianBrdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBrdfTest, ReflectanceTransmitted) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::LambertianBrdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBrdfTest, Reflectance) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::LambertianBrdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(M_1_PI, result->Reflectance(1.0), 0.0001);
}

TEST(LambertianBtdfTest, IsDiffuse) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::LambertianBtdf bxdf(reflector);
  EXPECT_TRUE(bxdf.IsDiffuse());
}

TEST(LambertianBtdfTest, SampleDiffuseAligned) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::LambertianBtdf bxdf(reflector);
  auto result = bxdf.SampleDiffuse(iris::Vector(0.0, 0.0, 1.0),
                                   iris::Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(LambertianBtdfTest, SampleDiffuseUnaligned) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::LambertianBtdf bxdf(reflector);
  auto result = bxdf.SampleDiffuse(iris::Vector(0.0, 0.0, 1.0),
                                   iris::Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->z, -0.000345266, 0.0001);
}

TEST(LambertianBtdfTest, Sample) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::LambertianBtdf bxdf(reflector);
  auto result = bxdf.Sample(iris::Vector(0.0, 0.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  ASSERT_TRUE(result);
  EXPECT_NEAR(result->direction.x, -0.707106709, 0.0001);
  EXPECT_NEAR(result->direction.y, -0.707106709, 0.0001);
  EXPECT_NEAR(result->direction.z, -0.000345266, 0.0001);
  EXPECT_FALSE(result->differentials);
}

TEST(LambertianBtdfTest, DiffusePdfReflected) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::LambertianBtdf bxdf(reflector);
  EXPECT_EQ(0.0,
            bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
                     iris::Vector(0.0, 0.0, 1.0), &bxdf,
                     iris::Bxdf::Hemisphere::BRDF));
}

TEST(LambertianBtdfTest, DiffusePdfTransmitted) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::LambertianBtdf bxdf(reflector);
  EXPECT_NEAR(
      M_1_PI,
      *bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                iris::Vector(0.0, 0.0, -1.0), &bxdf,
                iris::Bxdf::Hemisphere::BTDF),
      0.001);
}

TEST(LambertianBtdfTest, ReflectanceBrdf) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::LambertianBtdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBtdfTest, ReflectanceReflected) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::LambertianBtdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(LambertianBtdfTest, Reflectance) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::LambertianBtdf bxdf(reflector);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(M_1_PI, result->Reflectance(1.0), 0.0001);
}