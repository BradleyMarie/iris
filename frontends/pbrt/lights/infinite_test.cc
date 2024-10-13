#include "frontends/pbrt/lights/infinite.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::testing::ExitedWithCode;

std::string RunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "__main__/frontends/pbrt/lights/test_data/";
  return std::string("\"") + runfiles->Rlocation(base_path + path) +
         std::string("\"");
}

TEST(Infinite, Empty) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(
      *g_infinite_builder, tokenizer, std::filesystem::current_path(),
      spectrum_manager, texture_manager,
      static_cast<SpectrumManager&>(spectrum_manager), Matrix::Identity());
  EXPECT_TRUE(std::get<ReferenceCounted<EnvironmentalLight>>(result));
}

TEST(Infinite, BadFiletype) {
  std::stringstream input("\"string mapname\" " + RunfilePath("image.txt"));
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(
      BuildObject(
          *g_infinite_builder, tokenizer, std::filesystem::current_path(),
          spectrum_manager, texture_manager,
          static_cast<SpectrumManager&>(spectrum_manager), Matrix::Identity()),
      ExitedWithCode(EXIT_FAILURE), "ERROR: Unsupported image file type: .txt");
}

TEST(Infinite, NoExtension) {
  std::stringstream input("\"string mapname\" " + RunfilePath("image"));
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(BuildObject(*g_infinite_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager,
                          static_cast<SpectrumManager&>(spectrum_manager),
                          Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported image file \\(no extension\\): image");
}

TEST(Infinite, AllSpecifiedEXR) {
  std::stringstream input(
      "\"color scale\" [1 1 1] \"color L\" [1 1 1] \"integer samples\" 2 "
      "\"string mapname\" " +
      RunfilePath("image.exr"));
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(
      *g_infinite_builder, tokenizer, std::filesystem::current_path(),
      spectrum_manager, texture_manager,
      static_cast<SpectrumManager&>(spectrum_manager), Matrix::Identity());
  EXPECT_TRUE(std::get<ReferenceCounted<EnvironmentalLight>>(result));
}

TEST(Infinite, Unimplemented) {
  std::stringstream input("\"string mapname\" " + RunfilePath("image.png"));
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(BuildObject(*g_infinite_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager,
                          static_cast<SpectrumManager&>(spectrum_manager),
                          Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unimplemented image file type: .png");
}

}  // namespace
}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris