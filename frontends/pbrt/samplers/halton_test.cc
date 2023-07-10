#include "frontends/pbrt/samplers/halton.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

TEST(Halton, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::samplers::g_halton_builder, tokenizer,
      spectrum_manager, texture_manager);
  EXPECT_TRUE(result);
}

TEST(Halton, TooLowPixelSamples) {
  std::stringstream input("\"integer pixelsamples\" -1");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::samplers::g_halton_builder, tokenizer,
                  spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: pixelsamples");
}

TEST(Halton, TooHighPixelSamples) {
  std::stringstream input("\"integer pixelsamples\" 4294967297");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::samplers::g_halton_builder, tokenizer,
                  spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: pixelsamples");
}

TEST(Halton, AllSpecified) {
  std::stringstream input("\"integer pixelsamples\" 4");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::samplers::g_halton_builder, tokenizer,
      spectrum_manager, texture_manager);
  EXPECT_TRUE(result);
}

TEST(Halton, Bounds) {
  std::stringstream input("\"integer pixelsamples\" 138085");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::samplers::g_halton_builder, tokenizer,
                  spectrum_manager, texture_manager),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Halton sampler only supports a maximum of 138084 "
              "samples per pixel");
}