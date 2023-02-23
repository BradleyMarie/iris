#include "frontends/pbrt/shapes/plymesh.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

TEST(PlyMesh, NoIndices) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_plymesh_builder, tokenizer,
                  spectrum_manager, texture_manager, material, normal, emissive,
                  emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required plymesh parameter: filename");
}

TEST(PlyMesh, Empty) {
  std::stringstream input(
      "\"string filename\" \"frontends/pbrt/shapes/test_data/empty.ply\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::shapes::g_plymesh_builder, tokenizer,
          spectrum_manager, texture_manager, material, normal, emissive,
          emissive, iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: Invalid PLY file specified by plymesh parameter: filename");
}

TEST(PlyMesh, Quads) {
  std::stringstream input(
      "\"string filename\" \"frontends/pbrt/shapes/test_data/quads.ply\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::shapes::g_plymesh_builder, tokenizer,
      spectrum_manager, texture_manager, material, normal, emissive, emissive,
      iris::Matrix::Identity());
  EXPECT_EQ(12u, result.first.size());
}

TEST(PlyMesh, Triangles) {
  std::stringstream input(
      "\"string filename\" \"frontends/pbrt/shapes/test_data/triangles.ply\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::shapes::g_plymesh_builder, tokenizer,
      spectrum_manager, texture_manager, material, normal, emissive, emissive,
      iris::Matrix::Identity());
  EXPECT_EQ(4u, result.first.size());
}