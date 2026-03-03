#include "frontends/pbrt/lights/spot.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/matrix.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::SpotLightSource;
using ::testing::ExitedWithCode;

TEST(Spot, Empty) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  SpotLightSource spot;

  EXPECT_TRUE(MakeSpot(spot, model_to_world, spectrum_manager));
}

}  // namespace
}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
