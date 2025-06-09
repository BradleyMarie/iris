#include "iris/cameras/orthographic_camera.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace cameras {
namespace {

TEST(OrthographicCameraTest, HasLens) {
  OrthographicCamera camera(Matrix::Identity(),
                            std::array<geometric_t, 2>({1.0, 1.0}));
  EXPECT_FALSE(camera.HasLens());
}

TEST(OrthographicCameraTest, NoTransform) {
  OrthographicCamera camera(Matrix::Identity(),
                            std::array<geometric_t, 2>({1.0, 1.0}));

  RayDifferential top_left =
      camera.Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_EQ(-1.0, top_left.origin.x);
  EXPECT_EQ(1.0, top_left.origin.y);
  EXPECT_EQ(0.0, top_left.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), top_left.direction);

  RayDifferential top_right =
      camera.Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_EQ(1.0, top_right.origin.x);
  EXPECT_EQ(1.0, top_right.origin.y);
  EXPECT_EQ(0.0, top_right.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), top_right.direction);

  RayDifferential bottom_left =
      camera.Compute({0.0, 1.0}, {0.0, 1.0}, std::nullopt);
  EXPECT_EQ(-1.0, bottom_left.origin.x);
  EXPECT_EQ(-1.0, bottom_left.origin.y);
  EXPECT_EQ(0.0, bottom_left.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), bottom_left.direction);

  RayDifferential bottom_right =
      camera.Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(1.0, bottom_right.origin.x);
  EXPECT_EQ(-1.0, bottom_right.origin.y);
  EXPECT_EQ(0.0, bottom_right.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), bottom_right.direction);
}

TEST(OrthographicCameraTest, Transformed) {
  OrthographicCamera camera(
      Matrix::Translation(1.0, 2.0, 3.0).value().Inverse(),
      std::array<geometric_t, 2>({0.5, 0.5}));

  RayDifferential top_left =
      camera.Compute({0.0, 0.0}, {0.0, 0.0}, std::nullopt);
  EXPECT_EQ(0.5, top_left.origin.x);
  EXPECT_EQ(2.5, top_left.origin.y);
  EXPECT_EQ(3.0, top_left.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), top_left.direction);

  RayDifferential top_right =
      camera.Compute({1.0, 0.0}, {1.0, 0.0}, std::nullopt);
  EXPECT_EQ(1.5, top_right.origin.x);
  EXPECT_EQ(2.5, top_right.origin.y);
  EXPECT_EQ(3.0, top_right.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), top_right.direction);

  RayDifferential bottom_left =
      camera.Compute({0.0, 1.0}, {0.0, 1.0}, std::nullopt);
  EXPECT_EQ(0.5, bottom_left.origin.x);
  EXPECT_EQ(1.5, bottom_left.origin.y);
  EXPECT_EQ(3.0, bottom_left.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), bottom_left.direction);

  RayDifferential bottom_right =
      camera.Compute({1.0, 1.0}, {1.0, 1.0}, std::nullopt);
  EXPECT_EQ(1.5, bottom_right.origin.x);
  EXPECT_EQ(1.5, bottom_right.origin.y);
  EXPECT_EQ(3.0, bottom_right.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), bottom_right.direction);
}

TEST(OrthographicCameraTest, Derivatives) {
  OrthographicCamera camera(
      Matrix::Translation(1.0, 2.0, 3.0).value().Inverse(),
      std::array<geometric_t, 2>({0.5, 0.5}));

  std::array<geometric_t, 2> derivatives({1.0, 1.0});
  RayDifferential top_left =
      camera.Compute({0.0, 0.0}, derivatives, std::nullopt);
  EXPECT_EQ(0.5, top_left.origin.x);
  EXPECT_EQ(2.5, top_left.origin.y);
  EXPECT_EQ(3.0, top_left.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), top_left.direction);
  ASSERT_TRUE(top_left.differentials);
  EXPECT_EQ(1.5, top_left.differentials->dx.origin.x);
  EXPECT_EQ(2.5, top_left.differentials->dx.origin.y);
  EXPECT_EQ(3.0, top_left.differentials->dx.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), top_left.differentials->dx.direction);
  EXPECT_EQ(0.5, top_left.differentials->dy.origin.x);
  EXPECT_EQ(1.5, top_left.differentials->dy.origin.y);
  EXPECT_EQ(3.0, top_left.differentials->dy.origin.z);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), top_left.differentials->dy.direction);
}

}  // namespace
}  // namespace cameras
}  // namespace iris
