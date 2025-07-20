#include "iris/bxdfs/internal/math.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/random/mock_random.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

using ::iris::random::MockRandom;
using ::testing::Return;

TEST(Math, CosTheta) { EXPECT_EQ(-0.5, CosTheta(Vector(0.8660, 0.0, -0.5))); }

TEST(Math, AbsCosTheta) {
  EXPECT_EQ(0.5, AbsCosTheta(Vector(0.8660, 0.0, -0.5)));
}

TEST(Math, CosSquaredTheta) {
  EXPECT_EQ(0.25, CosSquaredTheta(Vector(0.8660, 0.0, 0.5)));
}

TEST(Math, SinSquaredTheta) {
  EXPECT_EQ(0.75, SinSquaredTheta(Vector(0.8660, 0.0, 0.5)));
}

TEST(Math, SinTheta) {
  EXPECT_NEAR(std::sqrt(0.75), SinTheta(Vector(0.8660, 0.0, 0.5)), 0.001);
}

TEST(Math, TanTheta) {
  EXPECT_NEAR(std::sqrt(0.75) / 0.5, TanTheta(Vector(0.8660, 0.0, 0.5)), 0.001);
}

TEST(Math, TanSquaredTheta) {
  EXPECT_NEAR(0.75 / 0.25, TanSquaredTheta(Vector(0.8660, 0.0, 0.5)), 0.001);
}

TEST(Math, SinCosPhiZero) {
  std::pair<geometric_t, geometric_t> sin_cos =
      SinCosPhi(Vector(1.0, 0.0, 0.0));
  EXPECT_EQ(0.0, sin_cos.first);
  EXPECT_EQ(1.0, sin_cos.second);
}

TEST(Math, SinCosPhi) {
  std::pair<geometric_t, geometric_t> sin_cos =
      SinCosPhi(Vector(0.8660, 0.5, 0.0));
  EXPECT_NEAR(0.5, sin_cos.first, 0.001);
  EXPECT_NEAR(0.8660, sin_cos.second, 0.001);
}

TEST(Math, SinCosSquaredPhi) {
  std::pair<geometric_t, geometric_t> sin_cos =
      SinCosSquaredPhi(Vector(0.8660, 0.5, 0.0));
  EXPECT_NEAR(0.25, sin_cos.first, 0.001);
  EXPECT_NEAR(0.8660 * 0.8660, sin_cos.second, 0.001);
}

TEST(Math, CosDPhi) {
  EXPECT_NEAR(1.0, CosDPhi(Vector(1.0, 1.0, 2.0), Vector(1.0, 1.0, 2.0)),
              0.001);
  EXPECT_NEAR(-1.0, CosDPhi(Vector(1.0, 1.0, 2.0), Vector(-0.5, -0.5, 1.0)),
              0.001);
  EXPECT_NEAR(0.0, CosDPhi(Vector(1.0, 1.0, 2.0), Vector(1.0, -1.0, 2.0)),
              0.001);
  EXPECT_NEAR(1.0, CosDPhi(Vector(0.0, 0.0, 2.0), Vector(1.0, -1.0, 2.0)),
              0.001);
  EXPECT_NEAR(1.0, CosDPhi(Vector(1.0, -1.0, 2.0), Vector(0.0, 0.0, 2.0)),
              0.001);
}

TEST(Math, Reflect) {
  EXPECT_FALSE(Reflect(Vector(0.0, 0.0, 1.0), Vector(0.0, 0.0, -1.0)));
  Vector vector = Normalize(Vector(-1.0, -1.0, 1.0));
  std::optional<Vector> reflected = Reflect(vector, Vector(0.0, 0.0, 1.0));
  ASSERT_TRUE(reflected);
  EXPECT_EQ(-vector.x, reflected->x);
  EXPECT_EQ(-vector.y, reflected->y);
  EXPECT_EQ(vector.z, reflected->z);
}

TEST(Math, Refract) {
  Vector vector = Normalize(Vector(-1.0, -1.0, 1.0));
  std::optional<Vector> refracted = Refract(vector, Vector(0.0, 0.0, 1.0), 1.0);
  ASSERT_TRUE(refracted);
  EXPECT_EQ(-vector.x, refracted->x);
  EXPECT_EQ(-vector.y, refracted->y);
  EXPECT_EQ(-vector.z, refracted->z);

  std::optional<Vector> reflected =
      Refract(vector, Vector(0.0, 0.0, 1.0), 10.0);
  EXPECT_FALSE(reflected);
}

TEST(Math, HalfAngle) {
  EXPECT_FALSE(HalfAngle(Vector(1.0, 1.0, 1.0), Vector(-1.0, -1.0, -1.0)));
  std::optional<Vector> half_angle =
      HalfAngle(Vector(0.0, 0.0, 1.0), Vector(0.0, 1.0, 0.0));
  ASSERT_TRUE(half_angle);
  EXPECT_EQ(*half_angle, Normalize(Vector(0.0, 1.0, 1.0)));
}

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

TEST(ComputeCatmullRomWeights, Empty) {
  EXPECT_FALSE(ComputeCatmullRomWeights({}, 0.0));
}

TEST(ComputeCatmullRomWeights, OnePoint) {
  std::vector<geometric> numbers = {0.0};
  EXPECT_FALSE(ComputeCatmullRomWeights(numbers, -1.0));
  EXPECT_FALSE(ComputeCatmullRomWeights(numbers, 1.0));

  std::optional<CatmullRomWeights> on_point =
      ComputeCatmullRomWeights(numbers, 0.0);
  ASSERT_TRUE(on_point);
  ASSERT_EQ(on_point->num_weights, 1u);
  EXPECT_EQ(on_point->control_point_indices[0], 0u);
  EXPECT_EQ(on_point->control_point_weights[0], 1.0);
}

TEST(ComputeCatmullRomWeights, TwoPoints) {
  std::vector<geometric> numbers = {0.0, 1.0};

  std::optional<CatmullRomWeights> middle =
      ComputeCatmullRomWeights(numbers, 0.5);
  ASSERT_TRUE(middle);
  ASSERT_EQ(middle->num_weights, 2u);
  EXPECT_EQ(middle->control_point_indices[0], 0u);
  EXPECT_EQ(middle->control_point_indices[1], 1u);
  EXPECT_EQ(middle->control_point_weights[0], 0.5);
  EXPECT_EQ(middle->control_point_weights[1], 0.5);
}

TEST(ComputeCatmullRomWeights, ThreePoints) {
  std::vector<geometric> numbers = {-1.0, 0.0, 1.0};

  std::optional<CatmullRomWeights> near_start =
      ComputeCatmullRomWeights(numbers, -0.5);
  ASSERT_TRUE(near_start);
  ASSERT_EQ(near_start->num_weights, 3u);
  EXPECT_EQ(near_start->control_point_indices[0], 0u);
  EXPECT_EQ(near_start->control_point_indices[1], 1u);
  EXPECT_EQ(near_start->control_point_indices[2], 2u);
  EXPECT_EQ(near_start->control_point_weights[0], 0.4375);
  EXPECT_EQ(near_start->control_point_weights[1], 0.625);
  EXPECT_EQ(near_start->control_point_weights[2], -0.0625);

  std::optional<CatmullRomWeights> near_end =
      ComputeCatmullRomWeights(numbers, 0.5);
  ASSERT_TRUE(near_end);
  ASSERT_EQ(near_end->num_weights, 3u);
  EXPECT_EQ(near_end->control_point_indices[0], 0u);
  EXPECT_EQ(near_end->control_point_indices[1], 1u);
  EXPECT_EQ(near_end->control_point_indices[2], 2u);
  EXPECT_EQ(near_end->control_point_weights[0], -0.0625);
  EXPECT_EQ(near_end->control_point_weights[1], 0.625);
  EXPECT_EQ(near_end->control_point_weights[2], 0.4375);
}

TEST(ComputeCatmullRomWeights, WithNumbers) {
  std::vector<geometric> numbers = {-1.0, -0.75, -0.5, -0.25, 0.0,
                                    0.0,  0.25,  0.5,  0.75,  1.0};

  EXPECT_FALSE(ComputeCatmullRomWeights(numbers, -1.0001));

  std::optional<CatmullRomWeights> start =
      ComputeCatmullRomWeights(numbers, -1.0);
  ASSERT_TRUE(start);
  ASSERT_EQ(start->num_weights, 1u);
  EXPECT_EQ(start->control_point_indices[0], 0u);
  EXPECT_EQ(start->control_point_weights[0], 1.0);

  std::optional<CatmullRomWeights> near_start =
      ComputeCatmullRomWeights(numbers, -0.875);
  ASSERT_TRUE(near_start);
  ASSERT_EQ(near_start->num_weights, 3u);
  EXPECT_EQ(near_start->control_point_indices[0], 0u);
  EXPECT_EQ(near_start->control_point_indices[1], 1u);
  EXPECT_EQ(near_start->control_point_indices[2], 2u);
  EXPECT_EQ(near_start->control_point_weights[0], 0.4375);
  EXPECT_EQ(near_start->control_point_weights[1], 0.625);
  EXPECT_EQ(near_start->control_point_weights[2], -0.0625);

  std::optional<CatmullRomWeights> negative =
      ComputeCatmullRomWeights(numbers, -0.375);
  ASSERT_TRUE(negative);
  ASSERT_EQ(negative->num_weights, 4u);
  EXPECT_EQ(negative->control_point_indices[0], 1u);
  EXPECT_EQ(negative->control_point_indices[1], 2u);
  EXPECT_EQ(negative->control_point_indices[2], 3u);
  EXPECT_EQ(negative->control_point_indices[3], 4u);
  EXPECT_EQ(negative->control_point_weights[0], -0.0625);
  EXPECT_EQ(negative->control_point_weights[1], 0.5625);
  EXPECT_EQ(negative->control_point_weights[2], 0.5625);
  EXPECT_EQ(negative->control_point_weights[3], -0.0625);

  std::optional<CatmullRomWeights> negative_near_zero =
      ComputeCatmullRomWeights(numbers, -0.125);
  ASSERT_TRUE(negative_near_zero);
  ASSERT_EQ(negative_near_zero->num_weights, 3u);
  EXPECT_EQ(negative_near_zero->control_point_indices[0], 2u);
  EXPECT_EQ(negative_near_zero->control_point_indices[1], 3u);
  EXPECT_EQ(negative_near_zero->control_point_indices[2], 4u);
  EXPECT_EQ(negative_near_zero->control_point_weights[0], -0.0625);
  EXPECT_EQ(negative_near_zero->control_point_weights[1], 0.625);
  EXPECT_EQ(negative_near_zero->control_point_weights[2], 0.4375);

  std::optional<CatmullRomWeights> positive_near_zero =
      ComputeCatmullRomWeights(numbers, 0.125);
  ASSERT_TRUE(positive_near_zero);
  ASSERT_EQ(positive_near_zero->num_weights, 3u);
  EXPECT_EQ(positive_near_zero->control_point_indices[0], 5u);
  EXPECT_EQ(positive_near_zero->control_point_indices[1], 6u);
  EXPECT_EQ(positive_near_zero->control_point_indices[2], 7u);
  EXPECT_EQ(positive_near_zero->control_point_weights[0], 0.4375);
  EXPECT_EQ(positive_near_zero->control_point_weights[1], 0.625);
  EXPECT_EQ(positive_near_zero->control_point_weights[2], -0.0625);

  std::optional<CatmullRomWeights> positive =
      ComputeCatmullRomWeights(numbers, 0.375);
  ASSERT_TRUE(positive);
  ASSERT_EQ(positive->num_weights, 4u);
  EXPECT_EQ(positive->control_point_indices[0], 5u);
  EXPECT_EQ(positive->control_point_indices[1], 6u);
  EXPECT_EQ(positive->control_point_indices[2], 7u);
  EXPECT_EQ(positive->control_point_indices[3], 8u);
  EXPECT_EQ(positive->control_point_weights[0], -0.0625);
  EXPECT_EQ(positive->control_point_weights[1], 0.5625);
  EXPECT_EQ(positive->control_point_weights[2], 0.5625);
  EXPECT_EQ(positive->control_point_weights[3], -0.0625);

  std::optional<CatmullRomWeights> near_end =
      ComputeCatmullRomWeights(numbers, 0.875);
  ASSERT_TRUE(near_end);
  ASSERT_EQ(near_end->num_weights, 3u);
  EXPECT_EQ(near_end->control_point_indices[0], 7u);
  EXPECT_EQ(near_end->control_point_indices[1], 8u);
  EXPECT_EQ(near_end->control_point_indices[2], 9u);
  EXPECT_EQ(near_end->control_point_weights[0], -0.0625);
  EXPECT_EQ(near_end->control_point_weights[1], 0.625);
  EXPECT_EQ(near_end->control_point_weights[2], 0.4375);

  std::optional<CatmullRomWeights> end = ComputeCatmullRomWeights(numbers, 1.0);
  ASSERT_TRUE(end);
  ASSERT_EQ(end->num_weights, 1u);
  EXPECT_EQ(end->control_point_indices[0], 9u);
  EXPECT_EQ(end->control_point_weights[0], 1.0);

  EXPECT_FALSE(ComputeCatmullRomWeights(numbers, 1.0001));
}

}  // namespace
}  // namespace internal
}  // namespace bxdfs
}  // namespace iris
