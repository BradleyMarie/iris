#include "frontends/pbrt/lights/distant.h"

#include <cstdlib>
#include <iostream>

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

TEST(Distant, Empty) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Distant distant;

  EXPECT_TRUE(MakeDistant(distant, model_to_world, spectrum_manager));
}

TEST(Distant, SamePoint) {
  Matrix model_to_world = Matrix::Identity();
  TestSpectrumManager spectrum_manager;

  LightSource::Distant distant;
  distant.mutable_to()->set_x(1.0);
  distant.mutable_to()->set_y(1.0);
  distant.mutable_to()->set_z(1.0);
  distant.mutable_from()->set_x(1.0);
  distant.mutable_from()->set_y(1.0);
  distant.mutable_from()->set_z(1.0);

  EXPECT_EXIT(MakeDistant(distant, model_to_world, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: from and to must not be the same point");
}

}  // namespace
}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
