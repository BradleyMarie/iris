#define _USE_MATH_DEFINES
#include "iris/bxdfs/oren_nayar_bxdf.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"

TEST(OrenNayarBrdfTest, SampleAligned) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 0.1);
  auto result = bxdf.Sample(iris::Vector(0.0, 0.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, 1.0), sampler);
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), result->direction);
  EXPECT_FALSE(result->differentials);
}

TEST(OrenNayarBrdfTest, SampleUnaligned) {
  iris::reflectors::MockReflector reflector;
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.0));
  iris::Sampler sampler(rng);

  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 0.1);
  auto result = bxdf.Sample(iris::Vector(0.0, 0.0, 1.0), std::nullopt,
                            iris::Vector(0.0, 0.0, -1.0), sampler);
  EXPECT_EQ(iris::Vector(0.0, 0.0, -1.0), result->direction);
  EXPECT_FALSE(result->differentials);
}

TEST(OrenNayarBrdfTest, DiffusePdfTransmitted) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 0.1);
  EXPECT_EQ(0.0,
            bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0),
                     iris::Vector(0.0, 0.0, -1.0), &bxdf,
                     iris::Bxdf::Hemisphere::BTDF));
}

TEST(OrenNayarBrdfTest, DiffusePdfReflected) {
  iris::reflectors::MockReflector reflector;
  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 0.1);
  EXPECT_NEAR(
      M_1_PI,
      *bxdf.Pdf(iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0),
                iris::Vector(0.0, 0.0, -1.0), &bxdf,
                iris::Bxdf::Hemisphere::BRDF),
      0.001);
}

TEST(OrenNayarBrdfTest, ReflectanceBtdf) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 0.1);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(OrenNayarBrdfTest, ReflectanceTransmitted) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 0.1);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, -1.0), &bxdf,
      iris::Bxdf::Hemisphere::BTDF, iris::testing::GetSpectralAllocator());
  ASSERT_FALSE(result);
}

TEST(OrenNayarBrdfTest, ReflectanceVertical) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 0.1);
  auto* result = bxdf.Reflectance(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 0.0, 1.0), &bxdf,
      iris::Bxdf::Hemisphere::BRDF, iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(M_1_PI, result->Reflectance(1.0), 0.0001);
}

TEST(OrenNayarBrdfTest, ReflectanceIncomingDominant) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 10.0);
  auto* result = bxdf.Reflectance(iris::Normalize(iris::Vector(0.0, 1.0, 2.0)),
                                  iris::Normalize(iris::Vector(0.0, 1.0, 1.0)),
                                  &bxdf, iris::Bxdf::Hemisphere::BRDF,
                                  iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.31766638, result->Reflectance(1.0), 0.0001);
}

TEST(OrenNayarBrdfTest, ReflectanceOutgoingDominant) {
  iris::reflectors::MockReflector reflector;
  EXPECT_CALL(reflector, Reflectance(testing::_))
      .WillRepeatedly(testing::Return(1.0));

  iris::bxdfs::OrenNayarBrdf bxdf(reflector, 10.0);
  auto* result = bxdf.Reflectance(iris::Normalize(iris::Vector(0.0, 1.0, 1.0)),
                                  iris::Normalize(iris::Vector(0.0, 1.0, 2.0)),
                                  &bxdf, iris::Bxdf::Hemisphere::BRDF,
                                  iris::testing::GetSpectralAllocator());
  ASSERT_TRUE(result);
  EXPECT_NEAR(0.31766638, result->Reflectance(1.0), 0.0001);
}