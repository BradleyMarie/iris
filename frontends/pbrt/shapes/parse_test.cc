#include "frontends/pbrt/shapes/parse.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  spectrum_managers::TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(
      Parse(tokenizer, std::filesystem::current_path(), material_manager,
            spectrum_manager, texture_manager, nullptr,
            ReferenceCounted<EmissiveMaterial>(),
            ReferenceCounted<EmissiveMaterial>(), Matrix::Identity(), false),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Too few parameters to directive: Shape");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  spectrum_managers::TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(
      Parse(tokenizer, std::filesystem::current_path(), material_manager,
            spectrum_manager, texture_manager, nullptr,
            ReferenceCounted<EmissiveMaterial>(),
            ReferenceCounted<EmissiveMaterial>(), Matrix::Identity(), false),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Parameter to Shape must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  Tokenizer tokenizer(input);

  MaterialManager material_manager;
  spectrum_managers::TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  EXPECT_EXIT(
      Parse(tokenizer, std::filesystem::current_path(), material_manager,
            spectrum_manager, texture_manager, nullptr,
            ReferenceCounted<EmissiveMaterial>(),
            ReferenceCounted<EmissiveMaterial>(), Matrix::Identity(), false),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Unsupported type for directive Shape: NotAType");
}

// TODO: Add better coverage

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris