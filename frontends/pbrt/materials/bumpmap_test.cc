#include "frontends/pbrt/materials/bumpmap.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::FloatTextureParameter;

TEST(MakeBumpMap, Allocates) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  FloatTextureParameter float_texture_parameter;
  float_texture_parameter.set_float_value(1.0);

  std::array<ReferenceCounted<NormalMap>, 2> bump_maps =
      MakeBumpMap(float_texture_parameter, texture_manager);
  EXPECT_TRUE(bump_maps[0]);
  EXPECT_TRUE(bump_maps[1]);
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
