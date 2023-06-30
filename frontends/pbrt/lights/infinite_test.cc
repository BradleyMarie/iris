#include "frontends/pbrt/lights/infinite.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

TEST(Infinite, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
      spectrum_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      iris::Matrix::Identity());
  EXPECT_TRUE(
      std::get<iris::ReferenceCounted<iris::EnvironmentalLight>>(result));
}

TEST(Infinite, BadFiletype) {
  std::stringstream input(
      "\"string mapname\" \"frontends/pbrt/lights/test_data/image.txt\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
          spectrum_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported image file type: .txt");
}

TEST(Infinite, NoExtension) {
  std::stringstream input(
      "\"string mapname\" \"frontends/pbrt/lights/test_data/image\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
          spectrum_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported image file \\(no extension\\): image");
}

TEST(Infinite, AllSpecifiedEXR) {
  std::stringstream input(
      "\"spectrum L\" [1.0 1.0]"
      "\"string mapname\" \"frontends/pbrt/lights/test_data/image.exr\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
      spectrum_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      iris::Matrix::Identity());
  EXPECT_TRUE(
      std::get<iris::ReferenceCounted<iris::EnvironmentalLight>>(result));
}

TEST(Infinite, Unimplemented) {
  std::stringstream input(
      "\"string mapname\" \"frontends/pbrt/lights/test_data/image.png\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
          spectrum_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unimplemented image file type: .png");
}