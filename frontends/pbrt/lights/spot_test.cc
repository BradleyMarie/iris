#include "frontends/pbrt/lights/spot.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/matrix.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::LightSource;
using ::testing::ExitedWithCode;

TEST(Spot, Empty) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Spot point;

  EXPECT_TRUE(MakeSpot(point, model_to_world, spectrum_manager));
}

}  // namespace
}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
