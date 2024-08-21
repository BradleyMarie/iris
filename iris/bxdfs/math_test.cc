#include "iris/bxdfs/math.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace bxdfs {
namespace internal {

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

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris