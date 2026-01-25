#include "frontends/pbrt/shapes/plymesh.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/geometry/triangle_mesh.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"
#include "plyodine/readers/triangle_mesh_reader.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::iris::geometry::AllocateTriangleMesh;
using ::iris::textures::MaskTexture;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Shape;

class TriangleMeshReader final
    : public plyodine::TriangleMeshReader<geometric, geometric, geometric,
                                          uint32_t> {
 public:
  TriangleMeshReader(const Matrix& model_to_world)
      : model_to_world_(model_to_world) {}

  void AddVertex(const std::array<geometric, 3>& position,
                 const std::array<geometric, 3>* maybe_normal,
                 const std::array<geometric, 2>* maybe_uv) override {
    model_positions.emplace_back(position[0], position[1], position[2]);
    world_positions.emplace_back(
        model_to_world_.Multiply(model_positions.back()));

    if (maybe_normal) {
      model_normals.emplace_back((*maybe_normal)[0], (*maybe_normal)[1],
                                 (*maybe_normal)[2]);
      world_normals.emplace_back(
          model_to_world_.InverseTransposeMultiply(model_normals.back()));
    }

    if (maybe_uv) {
      uvs.emplace_back((*maybe_uv)[0], (*maybe_uv)[1]);
    }
  }

  void AddTriangle(const std::array<uint32_t, 3>& face) override {
    largest_index = std::max(largest_index, face[0]);
    largest_index = std::max(largest_index, face[1]);
    largest_index = std::max(largest_index, face[2]);
    faces.emplace_back(face[0], face[1], face[2]);
  }

  std::vector<Point> model_positions;
  std::vector<Point> world_positions;
  std::vector<Vector> model_normals;
  std::vector<Vector> world_normals;
  std::vector<std::pair<geometric, geometric>> uvs;
  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> faces;
  uint32_t largest_index = 0;

 private:
  const Matrix& model_to_world_;
};

}  // namespace

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> MakePlyMesh(
    const pbrt_proto::v3::Shape::PlyMesh& plymesh, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map,
    const std::filesystem::path& search_root, TextureManager& texture_manager,
    bool reversed_orientation) {
  Shape::PlyMesh with_defaults = Defaults().shapes().plymesh();
  with_defaults.MergeFrom(plymesh);

  std::filesystem::path path = with_defaults.filename();
  if (path.is_relative()) {
    path = search_root / path;
  }

  if (!std::filesystem::is_regular_file(path)) {
    std::cerr
        << "ERROR: Could not open file specified by plymesh parameter: filename"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file_stream(path, std::ios::in | std::ios::binary);
  if (file_stream.fail()) {
    std::cerr
        << "ERROR: Could not open file specified by plymesh parameter: filename"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  TriangleMeshReader reader(model_to_world);
  if (std::error_code error = reader.ReadFrom(file_stream); error) {
    std::cerr << "ERROR: PLY file parsing failed with message: "
              << error.message() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!reader.world_positions.empty() &&
      reader.world_positions.size() < reader.largest_index) {
    std::cerr << "ERROR: PLY model contained too few points to staisfy the "
                 "number of indexed vertices"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!reader.world_normals.empty() &&
      reader.world_normals.size() < reader.largest_index) {
    std::cerr << "ERROR: PLY model contained too few normal to staisfy the "
                 "number of indexed vertices"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!reader.uvs.empty() && reader.uvs.size() < reader.largest_index) {
    std::cerr << "ERROR: PLY model contained too few uv pairs to staisfy the "
                 "number of indexed vertices"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<textures::MaskTexture> alpha_mask =
      texture_manager.AllocateFloatTexture(with_defaults.alpha());
  if (!alpha_mask) {
    return std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>(
        {}, model_to_world);
  }

  if (!reader.model_normals.empty()) {
    for (std::tuple<uint32_t, uint32_t, uint32_t>& indices : reader.faces) {
      Vector surface_normal =
          CrossProduct(reader.model_positions[std::get<1>(indices)] -
                           reader.model_positions[std::get<0>(indices)],
                       reader.model_positions[std::get<2>(indices)] -
                           reader.model_positions[std::get<0>(indices)]);

      geometric_t cumulative_dp = DotProduct(
          surface_normal, reader.model_normals[std::get<0>(indices)] +
                              reader.model_normals[std::get<1>(indices)] +
                              reader.model_normals[std::get<2>(indices)]);

      if (cumulative_dp < static_cast<geometric_t>(0.0)) {
        std::swap(std::get<1>(indices), std::get<2>(indices));
      }

      if (model_to_world.SwapsHandedness() ^ reversed_orientation) {
        std::swap(std::get<1>(indices), std::get<2>(indices));
      }
    }
  } else if (model_to_world.SwapsHandedness()) {
    for (std::tuple<uint32_t, uint32_t, uint32_t>& indices : reader.faces) {
      std::swap(std::get<1>(indices), std::get<2>(indices));
    }
  }

  std::vector<ReferenceCounted<Geometry>> triangles = AllocateTriangleMesh(
      reader.world_positions, reader.faces, {}, reader.world_normals,
      reader.uvs, std::move(alpha_mask), front_material, back_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  return std::make_pair(std::move(triangles), Matrix::Identity());
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
