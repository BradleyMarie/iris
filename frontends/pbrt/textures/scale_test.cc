#include "frontends/pbrt/textures/scale.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

static const std::string kName = "name";

TEST(Scale, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_scale_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager, kName);
}

TEST(Scale, WithTex1) {
  std::stringstream input("\"float tex1\" 0.5");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_scale_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager, kName);
}

TEST(Scale, WithTex2) {
  std::stringstream input("\"float tex2\" 0.5");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_scale_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager, kName);
}

TEST(Scale, Full) {
  std::stringstream input("\"float tex1\" 0.5 \"float tex2\" 0.5");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::textures::g_float_scale_builder, tokenizer,
      spectrum_manager, texture_manager, texture_manager, kName);
}