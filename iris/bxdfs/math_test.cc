#include "iris/bxdfs/math.h"

#include "googletest/include/gtest/gtest.h"

TEST(Math, CosTheta) {
  EXPECT_EQ(-0.5,
            iris::bxdfs::internal::CosTheta(iris::Vector(0.8660, 0.0, -0.5)));
}

TEST(Math, AbsCosTheta) {
  EXPECT_EQ(
      0.5, iris::bxdfs::internal::AbsCosTheta(iris::Vector(0.8660, 0.0, -0.5)));
}

TEST(Math, CosSquaredTheta) {
  EXPECT_EQ(0.25, iris::bxdfs::internal::CosSquaredTheta(
                      iris::Vector(0.8660, 0.0, 0.5)));
}

TEST(Math, SinSquaredTheta) {
  EXPECT_EQ(0.75, iris::bxdfs::internal::SinSquaredTheta(
                      iris::Vector(0.8660, 0.0, 0.5)));
}

TEST(Math, SinTheta) {
  EXPECT_NEAR(std::sqrt(0.75),
              iris::bxdfs::internal::SinTheta(iris::Vector(0.8660, 0.0, 0.5)),
              0.001);
}

TEST(Math, TanTheta) {
  EXPECT_NEAR(std::sqrt(0.75) / 0.5,
              iris::bxdfs::internal::TanTheta(iris::Vector(0.8660, 0.0, 0.5)),
              0.001);
}

TEST(Math, TanSquaredTheta) {
  EXPECT_NEAR(
      0.75 / 0.25,
      iris::bxdfs::internal::TanSquaredTheta(iris::Vector(0.8660, 0.0, 0.5)),
      0.001);
}

TEST(Math, SinCosPhiZero) {
  auto sin_cos = iris::bxdfs::internal::SinCosPhi(iris::Vector(1.0, 0.0, 0.0));
  EXPECT_EQ(0.0, sin_cos.first);
  EXPECT_EQ(1.0, sin_cos.second);
}

TEST(Math, SinCosPhi) {
  auto sin_cos =
      iris::bxdfs::internal::SinCosPhi(iris::Vector(0.8660, 0.5, 0.0));
  EXPECT_NEAR(0.5, sin_cos.first, 0.001);
  EXPECT_NEAR(0.8660, sin_cos.second, 0.001);
}

TEST(Math, SinCosSquaredPhi) {
  auto sin_cos =
      iris::bxdfs::internal::SinCosSquaredPhi(iris::Vector(0.8660, 0.5, 0.0));
  EXPECT_NEAR(0.25, sin_cos.first, 0.001);
  EXPECT_NEAR(0.8660 * 0.8660, sin_cos.second, 0.001);
}

TEST(Math, Reflect) {
  EXPECT_FALSE(iris::bxdfs::internal::Reflect(iris::Vector(0.0, 0.0, 1.0),
                                              iris::Vector(0.0, 0.0, -1.0)));
  auto vector = iris::Normalize(iris::Vector(-1.0, -1.0, 1.0));
  auto reflected =
      iris::bxdfs::internal::Reflect(vector, iris::Vector(0.0, 0.0, 1.0));
  ASSERT_TRUE(reflected);
  EXPECT_EQ(-vector.x, reflected->x);
  EXPECT_EQ(-vector.y, reflected->y);
  EXPECT_EQ(vector.z, reflected->z);
}

TEST(Math, Refract) {
  auto vector = iris::Normalize(iris::Vector(-1.0, -1.0, 1.0));
  auto refracted =
      iris::bxdfs::internal::Refract(vector, iris::Vector(0.0, 0.0, 1.0), 1.0);
  ASSERT_TRUE(refracted);
  EXPECT_EQ(-vector.x, refracted->x);
  EXPECT_EQ(-vector.y, refracted->y);
  EXPECT_EQ(-vector.z, refracted->z);

  auto reflected =
      iris::bxdfs::internal::Refract(vector, iris::Vector(0.0, 0.0, 1.0), 10.0);
  EXPECT_FALSE(reflected);
}

TEST(Math, HalfAngle) {
  EXPECT_FALSE(iris::bxdfs::internal::HalfAngle(
      iris::Vector(1.0, 1.0, 1.0), iris::Vector(-1.0, -1.0, -1.0)));
  auto half_angle = iris::bxdfs::internal::HalfAngle(
      iris::Vector(0.0, 0.0, 1.0), iris::Vector(0.0, 1.0, 0.0));
  ASSERT_TRUE(half_angle);
  EXPECT_EQ(*half_angle, iris::Normalize(iris::Vector(0.0, 1.0, 1.0)));
}