#include "frontends/pbrt/textures/windy.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/matrix.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::WindyFloatTexture;
using ::pbrt_proto::WindySpectrumTexture;

TEST(Windy, FloatTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  WindyFloatTexture windy;
  EXPECT_TRUE(MakeWindy(windy, texture_manager, Matrix::Identity()));
}

TEST(Windy, SpectrumTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  WindySpectrumTexture windy;
  EXPECT_TRUE(MakeWindy(windy, texture_manager, Matrix::Identity()));
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
