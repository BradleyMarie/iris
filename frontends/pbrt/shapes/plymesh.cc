#include "frontends/pbrt/shapes/plymesh.h"

#include <fstream>
#include <vector>

#include "frontends/pbrt/shapes/alpha_adapter.h"
#include "iris/geometry/triangle_mesh.h"
#include "plyodine/readers/triangle_mesh_reader.h"

namespace iris::pbrt_frontend::shapes {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"alpha", Parameter::FLOAT_TEXTURE},
        {"filename", Parameter::FILE_PATH},
};

class TriangleMeshReader final
    : public plyodine::TriangleMeshReader<geometric, geometric, geometric,
                                          uint32_t> {
 public:
  TriangleMeshReader(const Matrix& model_to_world)
      : model_to_world_(model_to_world) {}

  void Start() override {}

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

  void AddFace(const std::array<uint32_t, 3>& face) override {
    faces.emplace_back(face[0], face[1], face[2]);
  }

  std::vector<Point> positions;
  std::vector<std::tuple<geometric, geometric, geometric>> normals;
  std::vector<std::pair<geometric, geometric>> uvs;
  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> faces;

 private:
  const Matrix& model_to_world_;
};

class PlyMeshBuilder
    : public ObjectBuilder<
          std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
          const ReferenceCounted<iris::Material>&,
          const ReferenceCounted<iris::Material>&,
          const ReferenceCounted<iris::NormalMap>&,
          const ReferenceCounted<iris::NormalMap>&,
          const ReferenceCounted<EmissiveMaterial>&,
          const ReferenceCounted<EmissiveMaterial>&, const Matrix&> {
 public:
  PlyMeshBuilder() : ObjectBuilder(g_parameters) {}

  std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const ReferenceCounted<iris::Material>& front_material,
      const ReferenceCounted<iris::Material>& back_material,
      const ReferenceCounted<iris::NormalMap>& front_normal_map,
      const ReferenceCounted<iris::NormalMap>& back_normal_map,
      const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
      const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
      const Matrix& model_to_world) const override;
};

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>
PlyMeshBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const ReferenceCounted<iris::Material>& front_material,
    const ReferenceCounted<iris::Material>& back_material,
    const ReferenceCounted<iris::NormalMap>& front_normal_map,
    const ReferenceCounted<iris::NormalMap>& back_normal_map,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const Matrix& model_to_world) const {
  auto filename_iter = parameters.find("filename");
  if (filename_iter == parameters.end()) {
    std::cerr << "ERROR: Missing required plymesh parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file_stream(filename_iter->second.GetFilePaths(1).front(),
                            std::ios::in | std::ios::binary);
  if (file_stream.fail()) {
    std::cerr
        << "ERROR: Could not open file specified by plymesh parameter: filename"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  TriangleMeshReader reader(model_to_world);
  auto result = reader.ReadFrom(file_stream);
  if (!result) {
    std::cerr << "ERROR: PLY file parsing failed with message: "
              << result.error() << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<textures::ValueTexture2D<bool>> alpha_mask;
  auto alpha = parameters.find("alpha");
  if (alpha != parameters.end()) {
    alpha_mask = MakeReferenceCounted<internal::AlphaAdapter>(
        alpha->second.GetFloatTextures().front());
  }

  auto triangles = iris::geometry::AllocateTriangleMesh(
      reader.positions, reader.faces, reader.normals, reader.uvs,
      std::move(alpha_mask), front_material, back_material,
      front_emissive_material, back_emissive_material, front_normal_map,
      back_normal_map);

  return std::make_pair(std::move(triangles), Matrix::Identity());
}

}  // namespace

extern const std::unique_ptr<const ObjectBuilder<
    std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
    const ReferenceCounted<iris::Material>&,
    const ReferenceCounted<iris::Material>&,
    const ReferenceCounted<iris::NormalMap>&,
    const ReferenceCounted<iris::NormalMap>&,
    const ReferenceCounted<EmissiveMaterial>&,
    const ReferenceCounted<EmissiveMaterial>&, const Matrix&>>
    g_plymesh_builder = std::make_unique<PlyMeshBuilder>();

}  // namespace iris::pbrt_frontend::shapes