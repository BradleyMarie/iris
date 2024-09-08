#include "frontends/pbrt/materials/mix.h"

#include <memory>
#include <sstream>

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/materials/mock_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MockMaterial;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;

static TextureManager g_texture_manager;

class TestObjectBuilder : public materials::NestedMaterialBuilder {
 public:
  TestObjectBuilder() : ObjectBuilder({}) {}

  materials::MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager,
      TextureManager& texture_manager) const override {
    ReferenceCounted<Material> material0 = MakeReferenceCounted<MockMaterial>();
    ReferenceCounted<Material> material1 = MakeReferenceCounted<MockMaterial>();
    ReferenceCounted<NormalMap> normal_map0;
    ReferenceCounted<NormalMap> normal_map1;
    return {material0, material1, normal_map0, normal_map1};
  }
};

TEST(Mix, Empty) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(
      BuildObject(*g_mix_builder, tokenizer, std::filesystem::current_path(),
                  spectrum_manager, texture_manager,
                  static_cast<const MaterialManager&>(material_manager),
                  texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Missing required mix parameter: namedmaterial1");
}

TEST(Mix, MissingOne) {
  std::stringstream input("\"string namedmaterial1\" \"a\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  material_manager.Put("a", std::make_shared<TestObjectBuilder>());

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(
      BuildObject(*g_mix_builder, tokenizer, std::filesystem::current_path(),
                  spectrum_manager, texture_manager,
                  static_cast<const MaterialManager&>(material_manager),
                  texture_manager),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Missing required mix parameter: namedmaterial2");
}

TEST(Mix, WithDefaults) {
  std::stringstream input(
      "\"string namedmaterial1\" \"a\" \"string namedmaterial2\" \"a\" "
      "\"float amount\" 0.5  \"float bumpmap\" 0.5");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  material_manager.Put("a", std::make_shared<TestObjectBuilder>());

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  std::shared_ptr<NestedMaterialBuilder> result0 = BuildObject(
      *g_mix_builder, tokenizer, std::filesystem::current_path(),
      spectrum_manager, texture_manager,
      static_cast<const MaterialManager&>(material_manager), texture_manager);
  EXPECT_TRUE(result0);

  MaterialBuilderResult result1 = BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, static_cast<const MaterialManager&>(material_manager),
      texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_TRUE(std::get<2>(result1));
  EXPECT_TRUE(std::get<3>(result1));

  MaterialBuilderResult result2 = BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, static_cast<const MaterialManager&>(material_manager),
      texture_manager);
  EXPECT_EQ(std::get<0>(result1), std::get<0>(result2));
  EXPECT_EQ(std::get<1>(result1), std::get<1>(result2));
  EXPECT_TRUE(std::get<2>(result2));
  EXPECT_TRUE(std::get<3>(result2));
}

TEST(Mix, OverridesDefaults) {
  std::stringstream input0(
      "\"string namedmaterial1\" \"a\" \"string namedmaterial2\" \"a\"");
  Tokenizer tokenizer0(input0);

  MaterialManager material_manager;
  material_manager.Put("a", std::make_shared<TestObjectBuilder>());
  material_manager.Put("b", std::make_shared<TestObjectBuilder>());

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  std::shared_ptr<NestedMaterialBuilder> result0 = BuildObject(
      *g_mix_builder, tokenizer0, std::filesystem::current_path(),
      spectrum_manager, texture_manager,
      static_cast<const MaterialManager&>(material_manager), texture_manager);
  EXPECT_TRUE(result0);

  MaterialBuilderResult result1 = BuildObject(
      *result0, tokenizer0, std::filesystem::current_path(), spectrum_manager,
      texture_manager, static_cast<const MaterialManager&>(material_manager),
      texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_FALSE(std::get<2>(result1));
  EXPECT_FALSE(std::get<3>(result1));

  std::stringstream input1(
      "\"string namedmaterial1\" \"b\" \"string namedmaterial2\" \"b\" "
      "\"float amount\" 0.5  \"float bumpmap\" 0.5");
  Tokenizer tokenizer1(input1);

  MaterialBuilderResult result2 = BuildObject(
      *result0, tokenizer1, std::filesystem::current_path(), spectrum_manager,
      texture_manager, static_cast<const MaterialManager&>(material_manager),
      texture_manager);
  EXPECT_NE(std::get<0>(result1), std::get<0>(result2));
  EXPECT_NE(std::get<1>(result1), std::get<1>(result2));
  EXPECT_TRUE(std::get<2>(result2));
  EXPECT_TRUE(std::get<3>(result2));
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris