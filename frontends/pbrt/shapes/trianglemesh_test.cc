#include "frontends/pbrt/shapes/trianglemesh.h"

#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::Shape;
using ::testing::ExitedWithCode;

TEST(MakeTriangleMesh, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;

  auto result = MakeTriangleMesh(
      trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
      ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
      ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
      ReferenceCounted<NormalMap>(), texture_manager, false);
  EXPECT_TRUE(result.first.empty());
}

TEST(MakeTriangleMesh, InvalidNormalCount) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_n();

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Invalid number of parameters in parameter list: N");
}

TEST(MakeTriangleMesh, InvalidUvCount) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_uv();

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Invalid number of parameters in parameter list: uv");
}

TEST(MakeTriangleMesh, IndexTooNegativeV0) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_p();
  trianglemesh.add_indices()->set_v0(-1);

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: indices");
}

TEST(MakeTriangleMesh, IndexTooHighV0) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_p();
  trianglemesh.add_indices()->set_v0(1);

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: indices");
}

TEST(MakeTriangleMesh, IndexTooNegativeV1) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_p();
  trianglemesh.add_indices()->set_v1(-1);

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: indices");
}

TEST(MakeTriangleMesh, IndexTooHighV1) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_p();
  trianglemesh.add_indices()->set_v1(1);

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: indices");
}

TEST(MakeTriangleMesh, IndexTooNegativeV2) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_p();
  trianglemesh.add_indices()->set_v2(-1);

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: indices");
}

TEST(MakeTriangleMesh, IndexTooHighV2) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  trianglemesh.add_p();
  trianglemesh.add_indices()->set_v2(1);

  EXPECT_EXIT(
      MakeTriangleMesh(
          trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
          ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
          ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
          ReferenceCounted<NormalMap>(), texture_manager, false),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Out of range value for parameter: indices");
}

TEST(MakeTriangleMesh, Succeeds) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);

  Shape::TriangleMesh trianglemesh;
  auto& v0 = *trianglemesh.add_p();
  v0.set_x(0.0);
  v0.set_y(0.0);
  v0.set_z(0.0);

  auto& v1 = *trianglemesh.add_p();
  v1.set_x(1.0);
  v1.set_y(0.0);
  v1.set_z(0.0);

  auto& v2 = *trianglemesh.add_p();
  v2.set_x(0.0);
  v2.set_y(1.0);
  v2.set_z(0.0);

  auto& indices = *trianglemesh.add_indices();
  indices.set_v0(0);
  indices.set_v1(1);
  indices.set_v2(2);

  auto result = MakeTriangleMesh(
      trianglemesh, Matrix::Identity(), ReferenceCounted<Material>(),
      ReferenceCounted<Material>(), ReferenceCounted<EmissiveMaterial>(),
      ReferenceCounted<EmissiveMaterial>(), ReferenceCounted<NormalMap>(),
      ReferenceCounted<NormalMap>(), texture_manager, false);
  EXPECT_EQ(1u, result.first.size());
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
