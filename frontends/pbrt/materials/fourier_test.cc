#include "frontends/pbrt/materials/fourier.h"

#include <filesystem>

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

TEST(MakeFourier, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  TextureManager::FloatTexturePtr bump =
      texture_manager.AllocateFloatTexture(1.0);
  texture_manager.Put("bump", bump);

  Material::Fourier fourier;
  fourier.mutable_bumpmap()->set_float_texture_name("bump");

  EXPECT_EXIT(
      MakeFourier(fourier, Shape::MaterialOverrides::default_instance(),
                  std::filesystem::current_path(), texture_manager,
                  spectrum_manager),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not open file specified by fourier parameter: bsdffile");
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
