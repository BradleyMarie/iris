#include "frontends/pbrt/shapes/sphere.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

TEST(Sphere, Empty) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::shapes::g_sphere_builder, tokenizer,
      std::filesystem::current_path(), spectrum_manager, texture_manager,
      material, material, normal, normal, emissive, emissive,
      iris::Matrix::Identity());
}

TEST(Sphere, WithRadius) {
  std::stringstream input("\"float radius\" 0.5");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::shapes::g_sphere_builder, tokenizer,
      std::filesystem::current_path(), spectrum_manager, texture_manager,
      material, material, normal, normal, emissive, emissive,
      iris::Matrix::Identity());
}