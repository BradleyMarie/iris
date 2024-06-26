#include "frontends/pbrt/lights/infinite.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

std::string RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "__main__/frontends/pbrt/lights/test_data/";
  return std::string("\"") + runfiles->Rlocation(base_path + path) +
         std::string("\"");
}

TEST(Infinite, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
      std::filesystem::current_path(), spectrum_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      iris::Matrix::Identity());
  EXPECT_TRUE(
      std::get<iris::ReferenceCounted<iris::EnvironmentalLight>>(result));
}

TEST(Infinite, BadScale) {
  std::stringstream input("\"color scale\" [1 1 1] \"color L\" [1 1 1]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
          std::filesystem::current_path(), spectrum_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Cannot specify parameters together: L, scale");
}

TEST(Infinite, BadFiletype) {
  std::stringstream input("\"string mapname\" " + RunfilePath("image.txt"));
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
          std::filesystem::current_path(), spectrum_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported image file type: .txt");
}

TEST(Infinite, NoExtension) {
  std::stringstream input("\"string mapname\" " + RunfilePath("image"));
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
          std::filesystem::current_path(), spectrum_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported image file \\(no extension\\): image");
}

TEST(Infinite, AllSpecifiedEXR) {
  std::stringstream input(
      "\"spectrum L\" [1.0 1.0] \"integer samples\" 2 \"string mapname\" " +
      RunfilePath("image.exr"));
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
      std::filesystem::current_path(), spectrum_manager, texture_manager,
      static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
      iris::Matrix::Identity());
  EXPECT_TRUE(
      std::get<iris::ReferenceCounted<iris::EnvironmentalLight>>(result));
}

TEST(Infinite, Unimplemented) {
  std::stringstream input("\"string mapname\" " + RunfilePath("image.png"));
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::lights::g_infinite_builder, tokenizer,
          std::filesystem::current_path(), spectrum_manager, texture_manager,
          static_cast<iris::pbrt_frontend::SpectrumManager&>(spectrum_manager),
          iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unimplemented image file type: .png");
}