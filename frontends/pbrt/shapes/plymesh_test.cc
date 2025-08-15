#include "frontends/pbrt/shapes/plymesh.h"

#include <filesystem>

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::bazel::tools::cpp::runfiles::Runfiles;
using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::Shape;
using ::testing::ExitedWithCode;

std::string RawRunfilePath(const std::string& path) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
  const char* base_path = "_main/frontends/pbrt/shapes/test_data/";
  return runfiles->Rlocation(base_path + path);
}

TEST(MakePlyMesh, BadFileName) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::PlyMesh plymesh;

  EXPECT_EXIT(
      MakePlyMesh(plymesh, Matrix::Identity(), ReferenceCounted<Material>(),
                  ReferenceCounted<Material>(),
                  ReferenceCounted<EmissiveMaterial>(),
                  ReferenceCounted<EmissiveMaterial>(),
                  ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>(),
                  std::filesystem::current_path(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not open file specified by plymesh parameter: filename");
}

TEST(MakePlyMesh, BadFile) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::PlyMesh plymesh;
  plymesh.set_filename(RawRunfilePath("empty.ply"));

  EXPECT_EXIT(
      MakePlyMesh(plymesh, Matrix::Identity(), ReferenceCounted<Material>(),
                  ReferenceCounted<Material>(),
                  ReferenceCounted<EmissiveMaterial>(),
                  ReferenceCounted<EmissiveMaterial>(),
                  ReferenceCounted<NormalMap>(), ReferenceCounted<NormalMap>(),
                  std::filesystem::current_path(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: PLY file parsing failed with message: The input must contain "
      "only 'ply' on its first line");
}

TEST(MakePlyMesh, Triangles) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::PlyMesh plymesh;
  plymesh.set_filename(RawRunfilePath("triangles.ply"));

  auto result = MakePlyMesh(
      plymesh, Matrix::Identity(), ReferenceCounted<Material>(),
      ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
      ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
      ReferenceCounted<NormalMap>(), std::filesystem::current_path(),
      texture_manager, false);
  EXPECT_EQ(4u, result.first.size());
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
