#define _USE_MATH_DEFINES
#include "iris/cameras/pinhole_camera.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace cameras {
namespace {

TEST(PinholeCameraTest, HasLens) {
  PinholeCamera camera(Matrix::Identity(),
                       std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4);
  EXPECT_FALSE(camera.HasLens());
}

TEST(PinholeCameraTest, FourCorners) {
  PinholeCamera camera(Matrix::Identity(),
                       std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4);

  RayDifferential top_left =
      camera.Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_left.origin);
  EXPECT_NEAR(-0.57735025882720947, top_left.direction.x, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.z, 0.001);

  RayDifferential top_right =
      camera.Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_right.origin);
  EXPECT_NEAR(0.57735025882720947, top_right.direction.x, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_right.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_right.direction.z, 0.001);

  RayDifferential bottom_left =
      camera.Compute({0.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), bottom_left.origin);
  EXPECT_NEAR(-0.57735025882720947, bottom_left.direction.x, 0.001);
  EXPECT_NEAR(-0.57735025882720947, bottom_left.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, bottom_left.direction.z, 0.001);

  RayDifferential bottom_right =
      camera.Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), bottom_right.origin);
  EXPECT_NEAR(0.57735025882720947, bottom_right.direction.x, 0.001);
  EXPECT_NEAR(-0.57735025882720947, bottom_right.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, bottom_right.direction.z, 0.001);
}

TEST(PinholeCameraTest, Center) {
  PinholeCamera camera(Matrix::Identity(),
                       std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4);
  RayDifferential ray = camera.Compute({0.5, 0.5}, {0.5, 0.5}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), ray.origin);
  EXPECT_NEAR(0.0, ray.direction.x, 0.001);
  EXPECT_NEAR(0.0, ray.direction.y, 0.001);
  EXPECT_NEAR(1.0, ray.direction.z, 0.001);
}

TEST(PinholeCameraTest, ScaledCenter) {
  PinholeCamera camera(Matrix::Scalar(2.0, 1.0, 2.0).value().Inverse(),
                       std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4);
  RayDifferential ray = camera.Compute({0.5, 0.5}, {0.5, 0.5}, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), ray.origin);
  EXPECT_NEAR(0.0, ray.direction.x, 0.001);
  EXPECT_NEAR(0.0, ray.direction.y, 0.001);
  EXPECT_NEAR(1.0, ray.direction.z, 0.001);
}

TEST(PinholeCameraTest, TranslatedCenter) {
  PinholeCamera camera(Matrix::Translation(1.0, 2.0, 3.0).value().Inverse(),
                       std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4);
  RayDifferential ray = camera.Compute({0.5, 0.5}, {0.5, 0.5}, std::nullopt);
  EXPECT_EQ(Point(1.0, 2.0, 3.0), ray.origin);
  EXPECT_NEAR(0.0, ray.direction.x, 0.001);
  EXPECT_NEAR(0.0, ray.direction.y, 0.001);
  EXPECT_NEAR(1.0, ray.direction.z, 0.001);
}

TEST(PinholeCameraTest, Differentials) {
  PinholeCamera camera(Matrix::Identity(),
                       std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4);

  std::array<geometric_t, 2> derivatives({1.0, 1.0});
  RayDifferential top_left =
      camera.Compute({0.0, 0.0}, derivatives, std::nullopt);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_left.origin);
  EXPECT_NEAR(-0.57735025882720947, top_left.direction.x, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.z, 0.001);
  ASSERT_TRUE(top_left.differentials);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_left.differentials->dx.origin);
  EXPECT_NEAR(0.57735025882720947, top_left.differentials->dx.direction.x,
              0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.differentials->dx.direction.y,
              0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.differentials->dx.direction.z,
              0.001);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), top_left.differentials->dy.origin);
  EXPECT_NEAR(-0.57735025882720947, top_left.differentials->dy.direction.x,
              0.001);
  EXPECT_NEAR(-0.57735025882720947, top_left.differentials->dy.direction.y,
              0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.differentials->dy.direction.z,
              0.001);
}

}  // namespace
}  // namespace cameras
}  // namespace iris