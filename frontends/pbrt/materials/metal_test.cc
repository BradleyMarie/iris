#include "frontends/pbrt/materials/metal.h"

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
using ::testing::ExitedWithCode;

TEST(MakeMetal, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  TextureManager::FloatTexturePtr bump =
      texture_manager.AllocateFloatTexture(1.0);
  texture_manager.Put("bump", bump);

  Material::Metal metal;
  metal.mutable_bumpmap()->set_float_texture_name("bump");

  MaterialResult result =
      MakeMetal(metal, Shape::MaterialOverrides::default_instance(),
                texture_manager, spectrum_manager);
  EXPECT_TRUE(result.material);
  EXPECT_TRUE(result.bumpmaps[0]);
  EXPECT_TRUE(result.bumpmaps[1]);
}

TEST(MakeMetal, KTextureNotSupported) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Material::Metal metal;
  metal.mutable_k()->set_spectrum_texture_name("tex");

  EXPECT_EXIT(MakeMetal(metal, Shape::MaterialOverrides::default_instance(),
                        texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Spectrum texture parameters are not implemented for "
              "metal Material");
}

TEST(MakeMetal, EtaTextureNotSupported) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Material::Metal metal;
  metal.mutable_eta()->set_spectrum_texture_name("tex");

  EXPECT_EXIT(MakeMetal(metal, Shape::MaterialOverrides::default_instance(),
                        texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Spectrum texture parameters are not implemented for "
              "metal Material");
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
