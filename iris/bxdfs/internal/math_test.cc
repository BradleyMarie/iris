#include "iris/bxdfs/internal/math.h"

#define _USE_MATH_CONSTANTS
#include <cmath>

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

TEST(FesnelDielectricReflectance, Refracted) {
  EXPECT_NEAR(0.04,
              iris::bxdfs::internal::FesnelDielectricReflectance(1.0, 1.0, 1.5),
              0.0001);
}

TEST(FesnelDielectricReflectance, TotalInternalReflection) {
  EXPECT_EQ(1.0,
            iris::bxdfs::internal::FesnelDielectricReflectance(0.1, 1.5, 1.0));
}

TEST(CosineSampleHemisphere, CenterUp) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.5));
  iris::Sampler sampler(rng);

  EXPECT_EQ(iris::Vector(0, 0, 1),
            iris::bxdfs::internal::CosineSampleHemisphere(0.5, sampler));
}

TEST(CosineSampleHemisphere, CenterDown) {
  iris::random::MockRandom rng;
  EXPECT_CALL(rng, NextGeometric())
      .Times(2)
      .WillRepeatedly(testing::Return(0.5));
  iris::Sampler sampler(rng);

  EXPECT_EQ(iris::Vector(0, 0, -1),
            iris::bxdfs::internal::CosineSampleHemisphere(-0.5, sampler));
}

TEST(CosineSampleHemisphere, FirstBiggerUp) {
  iris::random::MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
  }
  iris::Sampler sampler(rng);

  auto result = iris::bxdfs::internal::CosineSampleHemisphere(0.5, sampler);
  EXPECT_NEAR(result.x, -0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.y, 0.35355338454246521, 0.0001);
  EXPECT_NEAR(result.z, 0.86602538824081421, 0.0001);
}

TEST(CosineSampleHemisphere, SecondBiggerUp) {
  iris::random::MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.25));
  }
  iris::Sampler sampler(rng);

  auto result = iris::bxdfs::internal::CosineSampleHemisphere(0.5, sampler);
  EXPECT_NEAR(result.x, 0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.y, -0.353553384542465, 0.0001);
  EXPECT_NEAR(result.z, 0.8660253882408142, 0.0001);
}

TEST(CosineSampleHemisphere, FirstBiggerDown) {
  iris::random::MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
  }
  iris::Sampler sampler(rng);

  auto result = iris::bxdfs::internal::CosineSampleHemisphere(-0.5, sampler);
  EXPECT_NEAR(result.x, -0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.y, 0.35355338454246521, 0.0001);
  EXPECT_NEAR(result.z, -0.8660253882408142, 0.0001);
}

TEST(CosineSampleHemisphere, SecondBiggerDown) {
  iris::random::MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.75));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(testing::Return(0.25));
  }
  iris::Sampler sampler(rng);

  auto result = iris::bxdfs::internal::CosineSampleHemisphere(-0.5, sampler);
  EXPECT_NEAR(result.x, 0.35355338454246521, 0.0001);
  EXPECT_NEAR(result.y, -0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.z, -0.8660253882408142, 0.0001);
}