#include "frontends/pbrt/cameras/perspective.h"

#include <cstdlib>

#include "frontends/pbrt/matrix_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {
namespace {

using ::pbrt_proto::v3::Camera;
using ::testing::ExitedWithCode;

MatrixManager::Transformation Identity() {
  return {Matrix::Identity(), Matrix::Identity()};
}

TEST(Perspective, Empty) {
  Camera::Perspective orthographic;
  EXPECT_TRUE(MakePerspective(orthographic, Identity()));
}

TEST(Perspective, AspectRatioZero) {
  Camera::Perspective orthographic;
  orthographic.set_frameaspectratio(0.0);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Perspective, FocalDistanceZero) {
  Camera::Perspective orthographic;
  orthographic.set_focaldistance(0.0);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: focaldistance");
}

TEST(Perspective, FovTooLow) {
  Camera::Perspective orthographic;
  orthographic.set_fov(0.0);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, FovTooHigh) {
  Camera::Perspective orthographic;
  orthographic.set_fov(180.0);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, HalfTooLow) {
  Camera::Perspective orthographic;
  orthographic.set_halffov(0.0);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: halffov");
}

TEST(Perspective, HalfTooHigh) {
  Camera::Perspective orthographic;
  orthographic.set_halffov(90.0);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: halffov");
}

TEST(Perspective, LensRadiusTooLow) {
  Camera::Perspective orthographic;
  orthographic.set_lensradius(-0.1);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: lensradius");
}

TEST(Perspective, FovAndHalfFov) {
  Camera::Perspective orthographic;
  orthographic.set_fov(45.0);
  orthographic.set_halffov(45.0);

  EXPECT_EXIT(MakePerspective(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Cannot specify parameters together: fov, halffov");
}

}  // namespace
}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris