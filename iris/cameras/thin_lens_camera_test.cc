#define _USE_MATH_DEFINES
#include "iris/cameras/thin_lens_camera.h"

#include <cmath>

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace cameras {
namespace {

constexpr geometric kFocusDistance = 0.3;
constexpr geometric kAlmostZeroLensRadius = 0.000000000001;

TEST(ThinLensCameraTest, HasLens) {
  ThinLensCamera camera(Matrix::Identity(),
                        std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4,
                        kAlmostZeroLensRadius, kFocusDistance);
  EXPECT_TRUE(camera.HasLens());
}

TEST(ThinLensCameraTest, FourCorners) {
  ThinLensCamera camera(Matrix::Identity(),
                        std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4,
                        kAlmostZeroLensRadius, kFocusDistance);

  RayDifferential top_left =
      camera.Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_NEAR(0.0, top_left.origin.x, 0.001);
  EXPECT_NEAR(0.0, top_left.origin.y, 0.001);
  EXPECT_NEAR(0.0, top_left.origin.z, 0.001);
  EXPECT_NEAR(-0.57735025882720947, top_left.direction.x, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.z, 0.001);

  RayDifferential top_right =
      camera.Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_NEAR(0.0, top_right.origin.x, 0.001);
  EXPECT_NEAR(0.0, top_right.origin.y, 0.001);
  EXPECT_NEAR(0.0, top_right.origin.z, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_right.direction.x, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_right.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_right.direction.z, 0.001);

  RayDifferential bottom_left =
      camera.Compute({0.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_NEAR(0.0, bottom_left.origin.x, 0.001);
  EXPECT_NEAR(0.0, bottom_left.origin.y, 0.001);
  EXPECT_NEAR(0.0, bottom_left.origin.z, 0.001);
  EXPECT_NEAR(-0.57735025882720947, bottom_left.direction.x, 0.001);
  EXPECT_NEAR(-0.57735025882720947, bottom_left.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, bottom_left.direction.z, 0.001);

  RayDifferential bottom_right =
      camera.Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_NEAR(0.0, bottom_right.origin.x, 0.001);
  EXPECT_NEAR(0.0, bottom_right.origin.y, 0.001);
  EXPECT_NEAR(0.0, bottom_right.origin.z, 0.001);
  EXPECT_NEAR(0.57735025882720947, bottom_right.direction.x, 0.001);
  EXPECT_NEAR(-0.57735025882720947, bottom_right.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, bottom_right.direction.z, 0.001);
}

TEST(ThinLensCameraTest, Center) {
  ThinLensCamera camera(Matrix::Identity(),
                        std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4,
                        kAlmostZeroLensRadius, kFocusDistance);
  RayDifferential ray = camera.Compute({0.5, 0.5}, {0.5, 0.5}, std::nullopt);
  EXPECT_NEAR(0.0, ray.origin.x, 0.001);
  EXPECT_NEAR(0.0, ray.origin.y, 0.001);
  EXPECT_NEAR(0.0, ray.origin.z, 0.001);
  EXPECT_NEAR(0.0, ray.direction.x, 0.001);
  EXPECT_NEAR(0.0, ray.direction.y, 0.001);
  EXPECT_NEAR(1.0, ray.direction.z, 0.001);
}

TEST(ThinLensCameraTest, ScaledCenter) {
  ThinLensCamera camera(Matrix::Scalar(2.0, 1.0, 2.0).value().Inverse(),
                        std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4,
                        kAlmostZeroLensRadius, kFocusDistance);
  RayDifferential ray = camera.Compute({0.5, 0.5}, {0.5, 0.5}, std::nullopt);
  EXPECT_NEAR(0.0, ray.origin.x, 0.001);
  EXPECT_NEAR(0.0, ray.origin.y, 0.001);
  EXPECT_NEAR(0.0, ray.origin.z, 0.001);
  EXPECT_NEAR(0.0, ray.direction.x, 0.001);
  EXPECT_NEAR(0.0, ray.direction.y, 0.001);
  EXPECT_NEAR(1.0, ray.direction.z, 0.001);
}

TEST(ThinLensCameraTest, TranslatedCenter) {
  ThinLensCamera camera(Matrix::Translation(1.0, 2.0, 3.0).value().Inverse(),
                        std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4,
                        kAlmostZeroLensRadius, kFocusDistance);
  RayDifferential ray = camera.Compute({0.5, 0.5}, {0.5, 0.5}, std::nullopt);
  EXPECT_NEAR(1.0, ray.origin.x, 0.001);
  EXPECT_NEAR(2.0, ray.origin.y, 0.001);
  EXPECT_NEAR(3.0, ray.origin.z, 0.001);
  EXPECT_NEAR(0.0, ray.direction.x, 0.001);
  EXPECT_NEAR(0.0, ray.direction.y, 0.001);
  EXPECT_NEAR(1.0, ray.direction.z, 0.001);
}

TEST(ThinLensCameraTest, Differentials) {
  ThinLensCamera camera(Matrix::Identity(),
                        std::array<geometric_t, 2>({1.0, 1.0}), M_PI_4,
                        kAlmostZeroLensRadius, kFocusDistance);

  std::array<geometric_t, 2> derivatives({1.0, 1.0});
  RayDifferential top_left =
      camera.Compute({0.0, 0.0}, derivatives, std::nullopt);
  EXPECT_NEAR(0.0, top_left.origin.x, 0.001);
  EXPECT_NEAR(0.0, top_left.origin.y, 0.001);
  EXPECT_NEAR(0.0, top_left.origin.z, 0.001);
  EXPECT_NEAR(-0.57735025882720947, top_left.direction.x, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.y, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.direction.z, 0.001);
  ASSERT_TRUE(top_left.differentials);
  EXPECT_NEAR(0.0, top_left.differentials->dx.origin.x, 0.001);
  EXPECT_NEAR(0.0, top_left.differentials->dx.origin.y, 0.001);
  EXPECT_NEAR(0.0, top_left.differentials->dx.origin.z, 0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.differentials->dx.direction.x,
              0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.differentials->dx.direction.y,
              0.001);
  EXPECT_NEAR(0.57735025882720947, top_left.differentials->dx.direction.z,
              0.001);
  EXPECT_NEAR(0.0, top_left.differentials->dy.origin.x, 0.001);
  EXPECT_NEAR(0.0, top_left.differentials->dy.origin.y, 0.001);
  EXPECT_NEAR(0.0, top_left.differentials->dy.origin.z, 0.001);
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