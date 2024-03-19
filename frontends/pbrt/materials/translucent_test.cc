#include "frontends/pbrt/materials/translucent.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

static iris::pbrt_frontend::TextureManager g_texture_manager;

TEST(Translucent, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result0 = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::materials::g_translucent_builder, tokenizer,
      std::filesystem::current_path(), spectrum_manager, texture_manager,
      texture_manager);
  EXPECT_TRUE(result0);

  auto result1 = iris::pbrt_frontend::BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_FALSE(std::get<2>(result1));
  EXPECT_FALSE(std::get<3>(result1));

  auto result2 = iris::pbrt_frontend::BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, texture_manager);
  EXPECT_EQ(std::get<0>(result1), std::get<0>(result2));
  EXPECT_EQ(std::get<1>(result1), std::get<1>(result2));
  EXPECT_FALSE(std::get<2>(result1));
  EXPECT_FALSE(std::get<3>(result1));
}

TEST(Translucent, WithDefaults) {
  std::stringstream input(
      "\"float Kd\" 1.0 \"float Ks\" 1.0 \"float reflect\" 1.0 \"float "
      "transmit\" 1.0 \"float roughness\" 0.5 \"bool remaproughness\" "
      "\"false\" \"float bumpmap\" 0.5");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result0 = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::materials::g_translucent_builder, tokenizer,
      std::filesystem::current_path(), spectrum_manager, texture_manager,
      texture_manager);
  EXPECT_TRUE(result0);

  auto result1 = iris::pbrt_frontend::BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_TRUE(std::get<2>(result1));
  EXPECT_TRUE(std::get<3>(result1));

  auto result2 = iris::pbrt_frontend::BuildObject(
      *result0, tokenizer, std::filesystem::current_path(), spectrum_manager,
      texture_manager, texture_manager);
  EXPECT_EQ(std::get<0>(result1), std::get<0>(result2));
  EXPECT_EQ(std::get<1>(result1), std::get<1>(result2));
  EXPECT_TRUE(std::get<2>(result1));
  EXPECT_TRUE(std::get<3>(result1));
}

TEST(Translucent, OverridesDefaults) {
  std::stringstream input0("");
  iris::pbrt_frontend::Tokenizer tokenizer0(input0);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result0 = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::materials::g_translucent_builder, tokenizer0,
      std::filesystem::current_path(), spectrum_manager, texture_manager,
      texture_manager);
  EXPECT_TRUE(result0);

  auto result1 = iris::pbrt_frontend::BuildObject(
      *result0, tokenizer0, std::filesystem::current_path(), spectrum_manager,
      texture_manager, texture_manager);
  EXPECT_TRUE(std::get<0>(result1));
  EXPECT_TRUE(std::get<1>(result1));
  EXPECT_FALSE(std::get<2>(result1));
  EXPECT_FALSE(std::get<3>(result1));

  std::stringstream input1(
      "\"float Kd\" 1.0 \"float Ks\" 1.0 \"float reflect\" 1.0 \"float "
      "transmit\" 1.0 \"float roughness\" 0.5 \"bool remaproughness\" "
      "\"false\" \"float bumpmap\" 0.5");
  iris::pbrt_frontend::Tokenizer tokenizer1(input1);

  auto result2 = iris::pbrt_frontend::BuildObject(
      *result0, tokenizer1, std::filesystem::current_path(), spectrum_manager,
      texture_manager, texture_manager);
  EXPECT_NE(std::get<0>(result1), std::get<0>(result2));
  EXPECT_NE(std::get<1>(result1), std::get<1>(result2));
  EXPECT_TRUE(std::get<2>(result2));
  EXPECT_TRUE(std::get<3>(result2));
}