#include "frontends/pbrt/shapes/plymesh.h"

#include <fstream>
#include <utility>
#include <vector>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/shapes/alpha_adapter.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/geometry/triangle_mesh.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "plyodine/readers/triangle_mesh_reader.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::iris::geometry::AllocateTriangleMesh;
using ::iris::textures::ValueTexture2D;
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
    positions.emplace_back(
        model_to_world_.Multiply(Point(position[0], position[1], position[2])));

    if (maybe_normal) {
      geometric x = (*maybe_normal)[0];
      geometric y = (*maybe_normal)[1];
      geometric z = (*maybe_normal)[2];

      if (x != static_cast<geometric>(0.0) ||
          y != static_cast<geometric>(0.0) ||
          z != static_cast<geometric>(0.0)) {
        Vector normal(x, y, z);
        Vector transformed = model_to_world_.InverseTransposeMultiply(normal);
        x = transformed.x;
        y = transformed.y;
        z = transformed.z;
      }

      normals.emplace_back(x, y, z);
    }

    if (maybe_uv) {
      uvs.emplace_back((*maybe_uv)[0], (*maybe_uv)[1]);
    }
  }

  void AddTriangle(const std::array<uint32_t, 3>& face) override {
    faces.emplace_back(face[0], face[1], face[2]);
  }

  std::vector<Point> positions;
  std::vector<std::tuple<geometric, geometric, geometric>> normals;
  std::vector<std::pair<geometric, geometric>> uvs;
  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> faces;

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
    const std::filesystem::path& search_root, TextureManager& texture_manager) {
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

  ReferenceCounted<ValueTexture2D<bool>> alpha_mask;
  if (with_defaults.has_alpha() && with_defaults.alpha().float_value() <= 1.0) {
    alpha_mask = MakeReferenceCounted<AlphaAdapter>(
        texture_manager.AllocateFloatTexture(with_defaults.alpha()));
  }

  std::vector<ReferenceCounted<Geometry>> triangles;
  if (model_to_world.SwapsHandedness()) {
    triangles = AllocateTriangleMesh(
        reader.positions, reader.faces, reader.normals, reader.uvs,
        std::move(alpha_mask), back_material, front_material,
        back_emissive_material, front_emissive_material, back_normal_map,
        front_normal_map);
  } else {
    triangles = AllocateTriangleMesh(
        reader.positions, reader.faces, reader.normals, reader.uvs,
        std::move(alpha_mask), front_material, back_material,
        front_emissive_material, back_emissive_material, front_normal_map,
        back_normal_map);
  }

  return std::make_pair(std::move(triangles), Matrix::Identity());
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris