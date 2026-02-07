#include "frontends/pbrt/cameras/orthographic.h"

#include <cstdlib>

#include "frontends/pbrt/matrix_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {
namespace {

using ::pbrt_proto::OrthographicCamera;
using ::testing::ExitedWithCode;

MatrixManager::Transformation Identity() {
  return {Matrix::Identity(), Matrix::Identity()};
}

TEST(Orthographic, Empty) {
  OrthographicCamera orthographic;
  EXPECT_TRUE(MakeOrthographic(orthographic, Identity()));
}

TEST(Orthographic, AspectRatioZero) {
  OrthographicCamera orthographic;
  orthographic.set_frameaspectratio(0.0);

  EXPECT_EXIT(MakeOrthographic(orthographic, Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: frameaspectratio");
}

}  // namespace
}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris
