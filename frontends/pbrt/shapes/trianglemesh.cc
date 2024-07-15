#include "frontends/pbrt/shapes/trianglemesh.h"

#include "frontends/pbrt/shapes/alpha_adapter.h"
#include "iris/geometry/triangle_mesh.h"

namespace iris::pbrt_frontend::shapes {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"alpha", Parameter::FLOAT_TEXTURE}, {"indices", Parameter::INTEGER},
        {"N", Parameter::VECTOR3},           {"P", Parameter::POINT3},
        {"S", Parameter::VECTOR3},           {"uv", Parameter::FLOAT},
};

class TriangleMeshBuilder
    : public ObjectBuilder<
          std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
          const ReferenceCounted<iris::Material>&,
          const ReferenceCounted<iris::Material>&,
          const ReferenceCounted<iris::NormalMap>&,
          const ReferenceCounted<iris::NormalMap>&,
          const ReferenceCounted<EmissiveMaterial>&,
          const ReferenceCounted<EmissiveMaterial>&, const Matrix&> {
 public:
  TriangleMeshBuilder() : ObjectBuilder(g_parameters) {}

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
TriangleMeshBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const ReferenceCounted<iris::Material>& front_material,
    const ReferenceCounted<iris::Material>& back_material,
    const ReferenceCounted<iris::NormalMap>& front_normal_map,
    const ReferenceCounted<iris::NormalMap>& back_normal_map,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const Matrix& model_to_world) const {
  auto indices_iter = parameters.find("indices");
  if (indices_iter == parameters.end()) {
    std::cerr << "ERROR: Missing required trianglemesh parameter: indices"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& indices_array = indices_iter->second.GetIntegerValues(0u, 3u);
  if (indices_array.size() % 3u != 0u) {
    std::cerr
        << "ERROR: Invalid number of parameters in parameter list: indices"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  uint32_t max_index = 0u;

  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> indices;
  for (size_t i = 0; i < indices_array.size(); i += 3) {
    if (indices_array[i] > std::numeric_limits<uint32_t>::max() ||
        indices_array[i + 1] > std::numeric_limits<uint32_t>::max() ||
        indices_array[i + 2] > std::numeric_limits<uint32_t>::max()) {
      std::cerr << "ERROR: Out of range value for parameter: indices"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    if (indices_array[i] > max_index) {
      max_index = indices_array[i];
    }

    if (indices_array[i + 1] > max_index) {
      max_index = indices_array[i + 1];
    }

    if (indices_array[i + 2] > max_index) {
      max_index = indices_array[i + 2];
    }

    indices.emplace_back(static_cast<uint32_t>(indices_array[i]),
                         static_cast<uint32_t>(indices_array[i + 1]),
                         static_cast<uint32_t>(indices_array[i + 2]));
  }

  uint32_t target_size = max_index + 1u;

  auto p_iter = parameters.find("P");
  if (p_iter == parameters.end()) {
    std::cerr << "ERROR: Missing required trianglemesh parameter: P"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  const auto& points_array = p_iter->second.GetPoint3Values(0u, target_size);

  std::vector<Point> points;
  for (const auto& entry : points_array) {
    points.push_back(model_to_world.Multiply(entry));
  }

  ReferenceCounted<textures::ValueTexture2D<bool>> alpha_mask;
  auto alpha = parameters.find("alpha");
  if (alpha != parameters.end()) {
    alpha_mask = MakeReferenceCounted<internal::AlphaAdapter>(
        alpha->second.GetFloatTextures().front());
  }

  std::vector<std::tuple<geometric, geometric, geometric>> normals;
  auto n = parameters.find("N");
  if (n != parameters.end() && !n->second.GetVector3Values(0u, 0u).empty()) {
    auto normals_array = n->second.GetVector3Values(0u, target_size);

    for (const auto& normal : normals_array) {
      Vector transformed = model_to_world.InverseTransposeMultiply(normal);
      normals.emplace_back(transformed.x, transformed.y, transformed.z);
    }
  }

  std::vector<std::pair<geometric, geometric>> uvs;
  auto uv = parameters.find("uv");
  if (uv != parameters.end()) {
    auto& uv_array = uv->second.GetFloatValues(0u, 6u);
    if (uv_array.size() % 2 != 0 || uv_array.size() / 2 != points.size()) {
      std::cerr << "ERROR: Invalid number of parameters in parameter list: uv"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < uv_array.size(); i += 2) {
      uvs.emplace_back(static_cast<geometric>(uv_array[i]),
                       static_cast<geometric>(uv_array[i + 1]));
    }
  }

  std::vector<iris::ReferenceCounted<iris::Geometry>> triangles;
  if (model_to_world.SwapsHandedness()) {
    triangles = iris::geometry::AllocateTriangleMesh(
        points, indices, normals, uvs, std::move(alpha_mask), back_material,
        front_material, back_emissive_material, front_emissive_material,
        back_normal_map, front_normal_map);
  } else {
    triangles = iris::geometry::AllocateTriangleMesh(
        points, indices, normals, uvs, std::move(alpha_mask), front_material,
        back_material, front_emissive_material, back_emissive_material,
        front_normal_map, back_normal_map);
  }

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
    g_trianglemesh_builder = std::make_unique<TriangleMeshBuilder>();

}  // namespace iris::pbrt_frontend::shapes