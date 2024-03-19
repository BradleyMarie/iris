#include "frontends/pbrt/shapes/plymesh.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "__main__/frontends/pbrt/shapes/test_data/";
  return runfiles->Rlocation(base_path + path);
}

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
                  std::filesystem::current_path(), spectrum_manager,
                  texture_manager, material, material, normal, normal, emissive,
                  emissive, iris::Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required plymesh parameter: filename");
}

TEST(PlyMesh, Empty) {
  std::filesystem::path file_path(RawRunfilePath("empty.ply"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input("\"string filename\" \"empty.ply\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  EXPECT_EXIT(
      iris::pbrt_frontend::BuildObject(
          *iris::pbrt_frontend::shapes::g_plymesh_builder, tokenizer,
          search_path, spectrum_manager, texture_manager, material, material,
          normal, normal, emissive, emissive, iris::Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      "ERROR: PLY file parsing failed with message: The first line of "
      "the input must exactly contain the magic string");
}

TEST(PlyMesh, Quads) {
  std::filesystem::path file_path(RawRunfilePath("quads.ply"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input("\"string filename\" \"quads.ply\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::shapes::g_plymesh_builder, tokenizer, search_path,
      spectrum_manager, texture_manager, material, material, normal, normal,
      emissive, emissive, iris::Matrix::Identity());
  EXPECT_EQ(12u, result.first.size());
}

TEST(PlyMesh, Triangles) {
  std::filesystem::path file_path(RawRunfilePath("triangles.ply"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input(
      "\"float alpha\" [0.0] \"string filename\" \"triangles.ply\"");
  iris::pbrt_frontend::Tokenizer tokenizer(input);

  iris::pbrt_frontend::spectrum_managers::TestSpectrumManager spectrum_manager;
  iris::pbrt_frontend::TextureManager texture_manager;
  const iris::ReferenceCounted<iris::Material> material;
  const iris::ReferenceCounted<iris::NormalMap> normal;
  const iris::ReferenceCounted<iris::EmissiveMaterial> emissive;

  auto result = iris::pbrt_frontend::BuildObject(
      *iris::pbrt_frontend::shapes::g_plymesh_builder, tokenizer, search_path,
      spectrum_manager, texture_manager, material, material, normal, normal,
      emissive, emissive, iris::Matrix::Identity());
  EXPECT_EQ(4u, result.first.size());
}