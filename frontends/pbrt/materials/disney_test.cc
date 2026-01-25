#include "frontends/pbrt/materials/disney.h"

#include <cstdlib>

#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::testing::ExitedWithCode;

TEST(MakeDisney, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  ReferenceCounted<iris::textures::FloatTexture> bump =
      texture_manager.AllocateFloatTexture(1.0);
  texture_manager.Put("bump", bump);

  Material::Disney disney;
  disney.mutable_bumpmap()->set_float_texture_name("bump");

  EXPECT_EXIT(MakeDisney(disney, Shape::MaterialOverrides::default_instance(),
                         texture_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Material type: disney");
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
