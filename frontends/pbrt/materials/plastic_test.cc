#include "frontends/pbrt/materials/plastic.h"

#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;

TEST(MakePlastic, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  TextureManager::FloatTexturePtr bump =
      texture_manager.AllocateFloatTexture(1.0);
  texture_manager.Put("bump", bump);

  Material::Plastic plastic;
  plastic.mutable_bumpmap()->set_float_texture_name("bump");

  MaterialResult result = MakePlastic(
      plastic, Shape::MaterialOverrides::default_instance(), texture_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_TRUE(result.bumpmaps[0]);
  EXPECT_TRUE(result.bumpmaps[1]);
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
