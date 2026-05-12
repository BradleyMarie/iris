#include "frontends/pbrt/textures/scale.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::ScaleFloatTexture;
using ::pbrt_proto::ScaleSpectrumTexture;

TEST(Scale, FloatTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  ScaleFloatTexture scale;
  EXPECT_TRUE(MakeScale(scale, texture_manager));
}

TEST(Scale, SpectrumTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  ScaleSpectrumTexture scale;
  EXPECT_TRUE(MakeScale(scale, texture_manager));
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
