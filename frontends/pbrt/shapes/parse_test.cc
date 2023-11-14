#include "frontends/pbrt/shapes/parse.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

TEST(Parse, TooFewParameters) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(iris::pbrt_frontend::shapes::Parse(
                  tokenizer, spectrum_manager, texture_manager, nullptr,
                  iris::ReferenceCounted<iris::EmissiveMaterial>(),
                  iris::ReferenceCounted<iris::EmissiveMaterial>(),
                  iris::Matrix::Identity(), false),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Too few parameters to directive: Shape");
}

TEST(Parse, NotAString) {
  std::stringstream input("1.0");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(iris::pbrt_frontend::shapes::Parse(
                  tokenizer, spectrum_manager, texture_manager, nullptr,
                  iris::ReferenceCounted<iris::EmissiveMaterial>(),
                  iris::ReferenceCounted<iris::EmissiveMaterial>(),
                  iris::Matrix::Identity(), false),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Parameter to Shape must be a string");
}

TEST(Parse, InvalidType) {
  std::stringstream input("\"NotAType\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  EXPECT_EXIT(iris::pbrt_frontend::shapes::Parse(
                  tokenizer, spectrum_manager, texture_manager, nullptr,
                  iris::ReferenceCounted<iris::EmissiveMaterial>(),
                  iris::ReferenceCounted<iris::EmissiveMaterial>(),
                  iris::Matrix::Identity(), false),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported type for directive Shape: NotAType");
}

// TODO: Add better coverage