#include "frontends/pbrt/materials/parse.h"

#include <cstdlib>
#include <filesystem>

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/materials/mock_material.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::iris::materials::MockMaterial;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::testing::ExitedWithCode;

TEST(ParseMaterial, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_FALSE(result.materials[0]);
  EXPECT_FALSE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Disney) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_disney();

  EXPECT_EXIT(
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager),
      ExitedWithCode(EXIT_FAILURE), "ERROR: Unsupported Material type: disney");
}

TEST(ParseMaterial, Fourier) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_fourier();

  EXPECT_EXIT(
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not open file specified by fourier parameter: bsdffile");
}

TEST(ParseMaterial, Glass) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_glass();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Hair) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_hair();

  EXPECT_EXIT(
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager),
      ExitedWithCode(EXIT_FAILURE), "ERROR: Unsupported Material type: hair");
}

TEST(ParseMaterial, KdSubsurface) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_kdsubsurface();

  EXPECT_EXIT(
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported Material type: kdsubsurface");
}

TEST(ParseMaterial, Matte) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_matte();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Metal) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_metal();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Mirror) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_mirror();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Mix) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  MaterialResult a;
  a.materials[0] = MakeReferenceCounted<MockMaterial>();
  a.materials[1] = MakeReferenceCounted<MockMaterial>();
  material_manager.Put("a", {Material::default_instance(), a});

  Material material;
  material.mutable_mix()->set_namedmaterial1("a");
  material.mutable_mix()->set_namedmaterial2("a");

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Plastic) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_plastic();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Substrate) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_substrate();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Subsurface) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_subsurface();

  EXPECT_EXIT(
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported Material type: subsurface");
}

TEST(ParseMaterial, Translucent) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_translucent();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

TEST(ParseMaterial, Uber) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Material material;
  material.mutable_uber();

  MaterialResult result =
      ParseMaterial(material, Shape::MaterialOverrides::default_instance(),
                    std::filesystem::current_path(), material_manager,
                    texture_manager, spectrum_manager);
  EXPECT_TRUE(result.materials[0]);
  EXPECT_TRUE(result.materials[1]);
  EXPECT_FALSE(result.bumpmaps[0]);
  EXPECT_FALSE(result.bumpmaps[1]);
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
