#include "frontends/pbrt/shapes/trianglemesh.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"

TEST(TriangleMesh, NoIndices) {
  std::stringstream input("");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required trianglemesh parameter: indices");
}

TEST(TriangleMesh, InvalidIndicesCount) {
  std::stringstream input("\"integer indices\" [1 2 3 4]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid number of parameters in parameter list: indices");
}

TEST(TriangleMesh, OutOfRangeIndices0) {
  std::stringstream input("\"integer indices\" [4294967296 2 3]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: indices");
}

TEST(TriangleMesh, OutOfRangeIndices1) {
  std::stringstream input("\"integer indices\" [1 4294967296 2]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: indices");
}

TEST(TriangleMesh, OutOfRangeIndices2) {
  std::stringstream input("\"integer indices\" [1 2 4294967296]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: indices");
}

TEST(TriangleMesh, MissingP) {
  std::stringstream input("\"integer indices\" [1 2 3]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required trianglemesh parameter: P");
}

TEST(TriangleMesh, OddUv) {
  std::stringstream input(
      "\"integer indices\" [0 1 2] \"point P\" [1 2 3 4 5 6 7 8 9]"
      "\"float uv\" [1 2 3 4 5 6 7]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid number of parameters in parameter list: uv");
}

TEST(TriangleMesh, TooManyUv) {
  std::stringstream input(
      "\"integer indices\" [0 1 2] \"point P\" [1 2 3 4 5 6 7 8 9]"
      "\"float uv\" [1 2 3 4 5 6 7 8]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(iris::pbrt_frontend::BuildObject(
                  *iris::pbrt_frontend::shapes::g_trianglemesh_builder,
                  tokenizer, spectrum_manager, texture_manager, material,
                  normal, normal, emissive, emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid number of parameters in parameter list: uv");
}

TEST(TriangleMesh, AllParameters) {
  std::stringstream input(
      "\"float alpha\" [0.0]"
      "\"integer indices\" [0 1 2]"
      "\"point P\" [1 2 3 4 5 6 7 8 9]"
      "\"vector N\" [1 2 3 4 5 6 7 8 9]"
      "\"float uv\" [1 2 3 4 5 6]");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;
  const auto transformation = iris::Matrix::Translation(1.0, 0.0, 0.0).value();

  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::shapes::g_trianglemesh_builder, tokenizer,
      spectrum_manager, texture_manager, material, normal, normal, emissive,
      emissive, transformation);
  EXPECT_EQ(1u, result.first.size());
  EXPECT_EQ(iris::Matrix::Identity(), result.second);
}