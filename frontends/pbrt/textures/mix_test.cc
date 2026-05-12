#include "frontends/pbrt/textures/mix.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::MixFloatTexture;
using ::pbrt_proto::MixSpectrumTexture;

TEST(Mix, FloatTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  MixFloatTexture mix;
  EXPECT_TRUE(MakeMix(mix, texture_manager));
}

TEST(Mix, SpectrumTexture) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  MixSpectrumTexture mix;
  EXPECT_TRUE(MakeMix(mix, texture_manager));
}

}  // namespace
}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
