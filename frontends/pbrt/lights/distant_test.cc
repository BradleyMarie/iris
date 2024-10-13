#include "frontends/pbrt/lights/distant.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::testing::ExitedWithCode;

TEST(Distant, Empty) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(
      *g_distant_builder, tokenizer, std::filesystem::current_path(),
      spectrum_manager, texture_manager,
      static_cast<SpectrumManager&>(spectrum_manager), Matrix::Identity());
  EXPECT_TRUE(std::get<ReferenceCounted<Light>>(result));
}

TEST(Distant, SameAsFrom) {
  std::stringstream input("\"point to\" [0 0 0]");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(BuildObject(*g_distant_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager,
                          static_cast<SpectrumManager&>(spectrum_manager),
                          Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: from and to must not be the same point");
}

TEST(Distant, SameAsTo) {
  std::stringstream input("\"point from\" [0 0 1]");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(BuildObject(*g_distant_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager,
                          static_cast<SpectrumManager&>(spectrum_manager),
                          Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: from and to must not be the same point");
}

TEST(Distant, SamePoint) {
  std::stringstream input("\"point from\" [2 2 2] \"point to\" [2 2 2]");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;

  EXPECT_EXIT(BuildObject(*g_distant_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager,
                          static_cast<SpectrumManager&>(spectrum_manager),
                          Matrix::Identity()),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: from and to must not be the same point");
}

TEST(Distant, AllSpecified) {
  std::stringstream input(
      "\"spectrum L\" [1.0 1.0] \"point from\" [2 2 2] \"point to\" [3 3 3] "
      "\"spectrum scale\" [1.0 1.0]");
  Tokenizer tokenizer(input);

  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  auto result = BuildObject(
      *g_distant_builder, tokenizer, std::filesystem::current_path(),
      spectrum_manager, texture_manager,
      static_cast<SpectrumManager&>(spectrum_manager), Matrix::Identity());
  EXPECT_TRUE(std::get<ReferenceCounted<Light>>(result));
}

}  // namespace
}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris