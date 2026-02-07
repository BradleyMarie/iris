#include "frontends/pbrt/cameras/perspective.h"

#include <cstdlib>

#include "frontends/pbrt/matrix_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {
namespace {

using ::pbrt_proto::PerspectiveCamera;
using ::testing::ExitedWithCode;

MatrixManager::Transformation Identity() {
  return {Matrix::Identity(), Matrix::Identity()};
}

TEST(Perspective, Empty) {
  PerspectiveCamera perspective;
  EXPECT_TRUE(MakePerspective(perspective, Identity()));
}

TEST(Perspective, AspectRatioZero) {
  PerspectiveCamera perspective;
  perspective.set_frameaspectratio(0.0);

  EXPECT_EXIT(MakePerspective(perspective, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: frameaspectratio");
}

TEST(Perspective, FocalDistanceZero) {
  PerspectiveCamera perspective;
  perspective.set_focaldistance(0.0);

  EXPECT_EXIT(MakePerspective(perspective, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: focaldistance");
}

TEST(Perspective, FovTooLow) {
  PerspectiveCamera perspective;
  perspective.set_fov(0.0);

  EXPECT_EXIT(MakePerspective(perspective, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, FovTooHigh) {
  PerspectiveCamera perspective;
  perspective.set_fov(180.0);

  EXPECT_EXIT(MakePerspective(perspective, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: fov");
}

TEST(Perspective, LensRadiusTooLow) {
  PerspectiveCamera perspective;
  perspective.set_lensradius(-0.1);

  EXPECT_EXIT(MakePerspective(perspective, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: lensradius");
}

}  // namespace
}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris
