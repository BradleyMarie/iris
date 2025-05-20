#include "frontends/pbrt/materials/mix.h"

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/materials/mock_material.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MockMaterial;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;

TEST(MakeMix, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  MaterialResult a;
  a.materials[0] = MakeReferenceCounted<MockMaterial>();
  a.materials[1] = MakeReferenceCounted<MockMaterial>();
  material_manager.Put("a", {Material::default_instance(), a});

  Material::Mix mix;
  mix.set_namedmaterial1("a");
  mix.set_namedmaterial2("a");

  MaterialResult result =
      MakeMix(mix, Shape::MaterialOverrides::default_instance(),
              material_manager, texture_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
