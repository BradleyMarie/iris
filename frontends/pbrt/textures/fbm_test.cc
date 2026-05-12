#include "frontends/pbrt/textures/fbm.h"

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
using ::pbrt_proto::FBmFloatTexture;
using ::pbrt_proto::FBmSpectrumTexture;

TEST(Fbm, FloatTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  FBmFloatTexture fbm;
  EXPECT_TRUE(MakeFbm(fbm, texture_manager, Matrix::Identity()));
}

TEST(Fbm, SpectrumTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  FBmSpectrumTexture fbm;
  EXPECT_TRUE(MakeFbm(fbm, texture_manager, Matrix::Identity()));
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
