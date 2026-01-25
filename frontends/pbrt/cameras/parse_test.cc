#include "frontends/pbrt/cameras/parse.h"

#include <cstdlib>

#include "frontends/pbrt/matrix_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::pbrt_proto::v3::Camera;
using ::testing::ExitedWithCode;

MatrixManager::Transformation Identity() {
  return {Matrix::Identity(), Matrix::Identity()};
}

TEST(ParseCamera, Empty) {
  Camera camera;
  EXPECT_FALSE(ParseCamera(camera, Identity()));
}

TEST(ParseCamera, Environment) {
  Camera camera;
  camera.mutable_environment();

  EXPECT_EXIT(ParseCamera(camera, Identity()), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Camera type: environment");
}

TEST(ParseCamera, Orthographic) {
  Camera camera;
  camera.mutable_orthographic();

  EXPECT_TRUE(ParseCamera(camera, Identity()));
}

TEST(ParseCamera, Perspective) {
  Camera camera;
  camera.mutable_perspective();

  EXPECT_TRUE(ParseCamera(camera, Identity()));
}

TEST(ParseCamera, Realistic) {
  Camera camera;
  camera.mutable_realistic();

  EXPECT_EXIT(ParseCamera(camera, Identity()), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Camera type: realistic");
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
