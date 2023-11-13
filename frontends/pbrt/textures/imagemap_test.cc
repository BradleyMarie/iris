#include "frontends/pbrt/textures/imagemap.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

static const std::string kName = "name";

std::string RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "__main__/frontends/pbrt/textures/test_data/";
  return std::string("\"") + runfiles->Rlocation(base_path + path) +
         std::string("\"");
}

TEST(Image, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
          spectrum_manager, texture_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          kName),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Missing required image parameter: filename");
}

TEST(Image, BadFiletype) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.txt"));
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
          spectrum_manager, texture_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          kName),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported image file type: .txt");
}

TEST(Image, NoExtension) {
  std::stringstream input("\"string filename\" " + RunfilePath("image"));
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
          spectrum_manager, texture_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          kName),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported image file \\(no extension\\): image");
}

TEST(Image, BadUScale) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"float uscale\" 0.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
          spectrum_manager, texture_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          kName),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: uscale");
}

TEST(Image, BadVScale) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"float vscale\" 0.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
          spectrum_manager, texture_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          kName),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: vscale");
}

TEST(Image, BadWrapping) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"string wrap\" \"abc\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
          spectrum_manager, texture_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          kName),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: wrap");
}

TEST(Image, BlackWrapping) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"string wrap\" \"black\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      kName);
}

TEST(Image, ClampWrapping) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"string wrap\" \"clamp\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      kName);
}

TEST(Image, RepeatWrapping) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"string wrap\" \"repeat\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      kName);
}

TEST(Image, FloatAllParams) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"bool gamma\" \"false\""
                          " \"float scale\" 1.0"
                          " \"float udelta\" 0.0"
                          " \"float uscale\" 1.0"
                          " \"float vdelta\" 0.0"
                          " \"float vscale\" 1.0"
                          " \"string wrap\" \"repeat\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_imagemap_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      kName);
}

TEST(Image, SpectrumBadScale) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"float scale\" 2.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::textures::g_spectrum_imagemap_builder,
          tokenizer, spectrum_manager, texture_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          kName),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: scale");
}

TEST(Image, SpectrumAllParams) {
  std::stringstream input("\"string filename\" " + RunfilePath("image.png") +
                          " \"bool gamma\" \"false\""
                          " \"float scale\" 1.0"
                          " \"float udelta\" 0.0"
                          " \"float uscale\" 1.0"
                          " \"float vdelta\" 0.0"
                          " \"float vscale\" 1.0"
                          " \"string wrap\" \"repeat\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_spectrum_imagemap_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      kName);
}