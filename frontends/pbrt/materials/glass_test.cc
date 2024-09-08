#include "frontends/pbrt/materials/glass.h"

#include <memory>
#include <sstream>

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;

static const MaterialManager g_material_manager;
static TextureManager g_texture_manager;

TEST(Glass, Empty) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  std::shared_ptr<NestedMaterialBuilder> result0 = BuildObject(
      *g_glass_builder, tokenizer, std::filesystem::current_path(),
      spectrum_manager, texture_manager, g_material_manager, texture_manager);
  EXPECT_TRUE(result0);

  MaterialBuilderResult result1 = BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, g_material_manager, texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_FALSE(std::get<2>(result1));
  EXPECT_FALSE(std::get<3>(result1));

  MaterialBuilderResult result2 = BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, g_material_manager, texture_manager);
  EXPECT_EQ(std::get<0>(result1), std::get<0>(result2));
  EXPECT_EQ(std::get<1>(result1), std::get<1>(result2));
  EXPECT_FALSE(std::get<2>(result1));
  EXPECT_FALSE(std::get<3>(result1));
}

TEST(Glass, WithDefaults) {
  std::stringstream input(
      "\"float Kr\" 1.0 \"float Kt\" 1.0 \"float eta\" 1.5 \"float bumpmap\" "
      "0.5");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  std::shared_ptr<NestedMaterialBuilder> result0 = BuildObject(
      *g_glass_builder, tokenizer, std::filesystem::current_path(),
      spectrum_manager, texture_manager, g_material_manager, texture_manager);
  EXPECT_TRUE(result0);

  MaterialBuilderResult result1 = BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, g_material_manager, texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_TRUE(std::get<2>(result1));
  EXPECT_TRUE(std::get<3>(result1));

  MaterialBuilderResult result2 = BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, g_material_manager, texture_manager);
  EXPECT_EQ(std::get<0>(result1), std::get<0>(result2));
  EXPECT_EQ(std::get<1>(result1), std::get<1>(result2));
  EXPECT_TRUE(std::get<2>(result2));
  EXPECT_TRUE(std::get<3>(result2));
}

TEST(Glass, OverridesDefaults) {
  std::stringstream input0("");
  Tokenizer tokenizer0(input0);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  std::shared_ptr<NestedMaterialBuilder> result0 = BuildObject(
      *g_glass_builder, tokenizer0, std::filesystem::current_path(),
      spectrum_manager, texture_manager, g_material_manager, texture_manager);
  EXPECT_TRUE(result0);

  MaterialBuilderResult result1 = BuildObject(
      *result0, tokenizer0, std::filesystem::current_path(), spectrum_manager,
      texture_manager, g_material_manager, texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_FALSE(std::get<2>(result1));
  EXPECT_FALSE(std::get<3>(result1));

  std::stringstream input1(
      "\"float Kr\" 1.0 \"float Kt\" 1.0 \"float eta\" 1.5 \"float bumpmap\" "
      "0.5");
  Tokenizer tokenizer1(input1);

  MaterialBuilderResult result2 = BuildObject(
      *result0, tokenizer1, std::filesystem::current_path(), spectrum_manager,
      texture_manager, g_material_manager, texture_manager);
  EXPECT_NE(std::get<0>(result1), std::get<0>(result2));
  EXPECT_NE(std::get<1>(result1), std::get<1>(result2));
  EXPECT_TRUE(std::get<2>(result2));
  EXPECT_TRUE(std::get<3>(result2));
}

}  // namespace
}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris