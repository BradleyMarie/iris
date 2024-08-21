#include "iris/bxdfs/internal/math.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

using ::iris::random::MockRandom;
using ::testing::Return;

TEST(FesnelDielectricReflectance, Refracted) {
  EXPECT_NEAR(0.04, FesnelDielectricReflectance(1.0, 1.0, 1.5), 0.0001);
}

TEST(FesnelDielectricReflectance, TotalInternalReflection) {
  EXPECT_EQ(1.0, FesnelDielectricReflectance(0.1, 1.5, 1.0));
}

TEST(CosineSampleHemisphere, CenterUp) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.5));
  Sampler sampler(rng);

  EXPECT_EQ(Vector(0, 0, 1), CosineSampleHemisphere(0.5, sampler));
}

TEST(CosineSampleHemisphere, CenterDown) {
  MockRandom rng;
  EXPECT_CALL(rng, NextGeometric()).Times(2).WillRepeatedly(Return(0.5));
  Sampler sampler(rng);

  EXPECT_EQ(Vector(0, 0, -1), CosineSampleHemisphere(-0.5, sampler));
}

TEST(CosineSampleHemisphere, FirstBiggerUp) {
  MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
  }
  Sampler sampler(rng);

  Vector result = CosineSampleHemisphere(0.5, sampler);
  EXPECT_NEAR(result.x, -0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.y, 0.35355338454246521, 0.0001);
  EXPECT_NEAR(result.z, 0.86602538824081421, 0.0001);
}

TEST(CosineSampleHemisphere, SecondBiggerUp) {
  MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
  }
  Sampler sampler(rng);

  Vector result = CosineSampleHemisphere(0.5, sampler);
  EXPECT_NEAR(result.x, 0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.y, -0.353553384542465, 0.0001);
  EXPECT_NEAR(result.z, 0.8660253882408142, 0.0001);
}

TEST(CosineSampleHemisphere, FirstBiggerDown) {
  MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
  }
  Sampler sampler(rng);

  Vector result = CosineSampleHemisphere(-0.5, sampler);
  EXPECT_NEAR(result.x, -0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.y, 0.35355338454246521, 0.0001);
  EXPECT_NEAR(result.z, -0.8660253882408142, 0.0001);
}

TEST(CosineSampleHemisphere, SecondBiggerDown) {
  MockRandom rng;
  {
    testing::InSequence sequence;
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.75));
    EXPECT_CALL(rng, NextGeometric()).WillOnce(Return(0.25));
  }
  Sampler sampler(rng);

  Vector result = CosineSampleHemisphere(-0.5, sampler);
  EXPECT_NEAR(result.x, 0.35355338454246521, 0.0001);
  EXPECT_NEAR(result.y, -0.3535533845424652, 0.0001);
  EXPECT_NEAR(result.z, -0.8660253882408142, 0.0001);
}

}  // namespace
}  // namespace internal
}  // namespace bxdfs
}  // namespace iris