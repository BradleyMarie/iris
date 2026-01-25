#include "frontends/pbrt/integrators/path.h"

#include <cstdint>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace integrators {
namespace {

using ::pbrt_proto::v3::Integrator;
using ::testing::ExitedWithCode;

TEST(Path, Empty) {
  Integrator::Path path;
  EXPECT_TRUE(MakePath(path));
}

TEST(Path, MaxDepthTooHigh) {
  Integrator::Path path;
  path.set_maxdepth(256);

  EXPECT_EXIT(MakePath(path), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxdepth");
}

TEST(Path, NegativeRrthreshold) {
  Integrator::Path path;
  path.set_rrthreshold(-1.0);

  EXPECT_EXIT(MakePath(path), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: rrthreshold");
}

TEST(Path, InvalidPixelboundsX) {
  Integrator::Path path;
  path.mutable_pixelbounds()->set_x_min(1);
  path.mutable_pixelbounds()->set_x_max(0);
  path.mutable_pixelbounds()->set_y_min(0);
  path.mutable_pixelbounds()->set_y_max(1);

  EXPECT_EXIT(MakePath(path), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid values for parameter list: pixelbounds");
}

TEST(Path, InvalidPixelboundsY) {
  Integrator::Path path;
  path.mutable_pixelbounds()->set_x_min(0);
  path.mutable_pixelbounds()->set_x_max(1);
  path.mutable_pixelbounds()->set_y_min(1);
  path.mutable_pixelbounds()->set_y_max(0);

  EXPECT_EXIT(MakePath(path), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid values for parameter list: pixelbounds");
}

}  // namespace
}  // namespace integrators
}  // namespace pbrt_frontend
}  // namespace iris
