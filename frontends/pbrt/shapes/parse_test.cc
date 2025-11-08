#include "frontends/pbrt/shapes/parse.h"

#include <cstdlib>
#include <filesystem>
#include <utility>

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/spectrum_managers/test_spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::iris::pbrt_frontend::spectrum_managers::TestSpectrumManager;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::testing::ExitedWithCode;

TEST(ParseShape, Empty) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;

  auto result =
      ParseShape(shape, Matrix::Identity(), true, Material(), MaterialResult(),
                 {}, std::filesystem::current_path(), material_manager,
                 texture_manager, spectrum_manager);
  EXPECT_TRUE(result.first.empty());
}

TEST(ParseShape, Cone) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_cone();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: cone");
}

TEST(ParseShape, Curve) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_curve();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Incorrect number of values for parameter: p");
}

TEST(ParseShape, Cylinder) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_cylinder();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: cylinder");
}

TEST(ParseShape, Disk) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_disk();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: disk");
}

TEST(ParseShape, HeightField) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_heightfield();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: heightfield");
}

TEST(ParseShape, Hyperboloid) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_hyperboloid();
  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: hyperboloid");
}

TEST(ParseShape, LoopSubdiv) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_loopsubdiv();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: loopsubdiv");
}

TEST(ParseShape, Nurbs) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_nurbs();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: nurbs");
}

TEST(ParseShape, Paraboloid) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_paraboloid();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported Shape type: paraboloid");
}

TEST(ParseShape, PlyMesh) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_plymesh();

  EXPECT_EXIT(
      ParseShape(shape, Matrix::Identity(), true, Material(), MaterialResult(),
                 {}, std::filesystem::current_path(), material_manager,
                 texture_manager, spectrum_manager),
      ExitedWithCode(EXIT_FAILURE),
      "ERROR: Could not open file specified by plymesh parameter: filename");
}

TEST(ParseShape, Sphere) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  shape.mutable_sphere();

  auto result =
      ParseShape(shape, Matrix::Identity(), true, Material(), MaterialResult(),
                 {}, std::filesystem::current_path(), material_manager,
                 texture_manager, spectrum_manager);
  EXPECT_FALSE(result.first.empty());
}

TEST(ParseShape, TriangleMesh) {
  TestSpectrumManager spectrum_manager;
  TextureManager texture_manager(spectrum_manager);
  MaterialManager material_manager;

  Shape shape;
  auto& trianglemesh = *shape.mutable_trianglemesh();
  trianglemesh.add_indices();

  EXPECT_EXIT(ParseShape(shape, Matrix::Identity(), true, Material(),
                         MaterialResult(), {}, std::filesystem::current_path(),
                         material_manager, texture_manager, spectrum_manager),
              ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: indices");
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
