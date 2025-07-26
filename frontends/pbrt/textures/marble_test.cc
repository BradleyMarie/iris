#include "frontends/pbrt/textures/marble.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/matrix.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::SpectrumTexture;

TEST(Marble, SpectrumTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  SpectrumTexture::Marble marble;
  EXPECT_TRUE(MakeMarble(marble, texture_manager, Matrix::Identity()));
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
