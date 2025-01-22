#include "frontends/pbrt/shapes/plymesh.h"

#include "frontends/pbrt/build_objects.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::testing::StartsWith;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/shapes/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(PlyMesh, NoIndices) {
  std::stringstream input("");
  Tokenizer tokenizer(input);

  spectrum_managers::TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  const ReferenceCounted<Material> material;
  const ReferenceCounted<NormalMap> normal;
  const ReferenceCounted<EmissiveMaterial> emissive;

  EXPECT_EXIT(BuildObject(*g_plymesh_builder, tokenizer,
                          std::filesystem::current_path(), spectrum_manager,
                          texture_manager, material, material, normal, normal,
                          emissive, emissive, Matrix::Identity()),
              testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Missing required plymesh parameter: filename");
}

TEST(PlyMesh, Empty) {
  std::filesystem::path file_path(RawRunfilePath("empty.ply"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input("\"string filename\" \"empty.ply\"");
  Tokenizer tokenizer(input);

  spectrum_managers::TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  const ReferenceCounted<Material> material;
  const ReferenceCounted<NormalMap> normal;
  const ReferenceCounted<EmissiveMaterial> emissive;

  EXPECT_EXIT(
      BuildObject(*g_plymesh_builder, tokenizer, search_path, spectrum_manager,
                  texture_manager, material, material, normal, normal, emissive,
                  emissive, Matrix::Identity()),
      testing::ExitedWithCode(EXIT_FAILURE),
      StartsWith("ERROR: PLY file parsing failed with message"));
}

TEST(PlyMesh, Quads) {
  std::filesystem::path file_path(RawRunfilePath("quads.ply"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input("\"string filename\" \"quads.ply\"");
  Tokenizer tokenizer(input);

  spectrum_managers::TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  const ReferenceCounted<Material> material;
  const ReferenceCounted<NormalMap> normal;
  const ReferenceCounted<EmissiveMaterial> emissive;

  auto result =
      BuildObject(*g_plymesh_builder, tokenizer, search_path, spectrum_manager,
                  texture_manager, material, material, normal, normal, emissive,
                  emissive, Matrix::Identity());
  EXPECT_EQ(12u, result.first.size());
}

TEST(PlyMesh, Triangles) {
  std::filesystem::path file_path(RawRunfilePath("triangles.ply"));
  std::filesystem::path search_path =
      std::filesystem::weakly_canonical(file_path).parent_path();

  std::stringstream input(
      "\"float alpha\" [0.0] \"string filename\" \"triangles.ply\"");
  Tokenizer tokenizer(input);

  spectrum_managers::TestSpectrumManager spectrum_manager;
  TextureManager texture_manager;
  const ReferenceCounted<Material> material;
  const ReferenceCounted<NormalMap> normal;
  const ReferenceCounted<EmissiveMaterial> emissive;

  auto result =
      BuildObject(*g_plymesh_builder, tokenizer, search_path, spectrum_manager,
                  texture_manager, material, material, normal, normal, emissive,
                  emissive, Matrix::Identity());
  EXPECT_EQ(4u, result.first.size());
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris