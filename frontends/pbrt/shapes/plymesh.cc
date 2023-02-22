#include "frontends/pbrt/shapes/plymesh.h"

#include <fstream>

#include "iris/geometry/triangle_mesh.h"
#include "source/tinyply.h"

namespace iris::pbrt_frontend::shapes {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"alpha", Parameter::FLOAT_TEXTURE},
        {"filename", Parameter::FILE_PATH},
        {"shadowalpha", Parameter::FLOAT_TEXTURE},
};

template <typename ElementType, typename TupleType>
void AddToVector2(tinyply::PlyData& ply_data, std::vector<TupleType>& output) {
  for (size_t i = 0; i < ply_data.count * 2u; i += 2) {
    if (ply_data.t == tinyply::Type::INT8) {
      auto* entries =
          reinterpret_cast<const int8_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else if (ply_data.t == tinyply::Type::UINT8) {
      auto* entries =
          reinterpret_cast<const uint8_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else if (ply_data.t == tinyply::Type::INT16) {
      auto* entries =
          reinterpret_cast<const int16_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else if (ply_data.t == tinyply::Type::UINT16) {
      auto* entries =
          reinterpret_cast<const uint16_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else if (ply_data.t == tinyply::Type::INT32) {
      auto* entries =
          reinterpret_cast<const int32_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else if (ply_data.t == tinyply::Type::UINT32) {
      auto* entries =
          reinterpret_cast<const uint32_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else if (ply_data.t == tinyply::Type::FLOAT32) {
      auto* entries =
          reinterpret_cast<const float*>(ply_data.buffer.get_const());
      if (!std::isfinite(entries[i]) || !std::isfinite(entries[i + 1])) {
        std::cerr
            << "ERROR: Invalid PLY file specified by trianglemesh parameter: "
               "filename"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else if (ply_data.t == tinyply::Type::FLOAT64) {
      auto* entries =
          reinterpret_cast<const double*>(ply_data.buffer.get_const());
      if (!std::isfinite(entries[i]) || !std::isfinite(entries[i + 1])) {
        std::cerr
            << "ERROR: Invalid PLY file specified by trianglemesh parameter: "
               "filename"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]));
    } else {
      std::cerr
          << "ERROR: Invalid PLY file specified by trianglemesh parameter: "
             "filename"
          << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

template <typename ElementType, typename TupleType>
void AddToVector3(tinyply::PlyData& ply_data, std::vector<TupleType>& output) {
  for (size_t i = 0; i < ply_data.count * 3u; i += 2) {
    if (ply_data.t == tinyply::Type::INT8) {
      auto* entries =
          reinterpret_cast<const int8_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else if (ply_data.t == tinyply::Type::UINT8) {
      auto* entries =
          reinterpret_cast<const uint8_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else if (ply_data.t == tinyply::Type::INT16) {
      auto* entries =
          reinterpret_cast<const int16_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else if (ply_data.t == tinyply::Type::UINT16) {
      auto* entries =
          reinterpret_cast<const uint16_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else if (ply_data.t == tinyply::Type::INT32) {
      auto* entries =
          reinterpret_cast<const int32_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else if (ply_data.t == tinyply::Type::UINT32) {
      auto* entries =
          reinterpret_cast<const uint32_t*>(ply_data.buffer.get_const());
      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else if (ply_data.t == tinyply::Type::FLOAT32) {
      auto* entries =
          reinterpret_cast<const float*>(ply_data.buffer.get_const());
      if (!std::isfinite(entries[i]) ||
          !std::isfinite(entries[i + 1] || !std::isfinite(entries[i + 2]))) {
        std::cerr
            << "ERROR: Invalid PLY file specified by trianglemesh parameter: "
               "filename"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else if (ply_data.t == tinyply::Type::FLOAT64) {
      auto* entries =
          reinterpret_cast<const double*>(ply_data.buffer.get_const());
      if (!std::isfinite(entries[i]) ||
          !std::isfinite(entries[i + 1] || !std::isfinite(entries[i + 2]))) {
        std::cerr
            << "ERROR: Invalid PLY file specified by trianglemesh parameter: "
               "filename"
            << std::endl;
        exit(EXIT_FAILURE);
      }

      output.emplace_back(static_cast<ElementType>(entries[i]),
                          static_cast<ElementType>(entries[i + 1]),
                          static_cast<ElementType>(entries[i + 2]));
    } else {
      std::cerr
          << "ERROR: Invalid PLY file specified by trianglemesh parameter: "
             "filename"
          << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

class PlyMeshBuilder
    : public ObjectBuilder<
          std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
          const ReferenceCounted<iris::Material>&,
          const ReferenceCounted<iris::NormalMap>&,
          const ReferenceCounted<EmissiveMaterial>&,
          const ReferenceCounted<EmissiveMaterial>&, const Matrix&> {
 public:
  PlyMeshBuilder() : ObjectBuilder(g_parameters) {}

  std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const ReferenceCounted<iris::Material>& material,
      const ReferenceCounted<iris::NormalMap>& normal_map,
      const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
      const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
      const Matrix& model_to_world) const override;
};

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>
PlyMeshBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const ReferenceCounted<iris::Material>& material,
    const ReferenceCounted<iris::NormalMap>& normal_map,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const Matrix& model_to_world) const {
  auto filename_iter = parameters.find("filename");
  if (filename_iter == parameters.end()) {
    std::cerr << "ERROR: Missing required trianglemesh parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file_stream(filename_iter->second.GetFilePaths(1).front());
  if (file_stream.fail()) {
    std::cerr << "ERROR: Could not open file specified by trianglemesh "
                 "parameter: filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  tinyply::PlyFile plyfile;
  plyfile.parse_header(file_stream);

  bool has_x = false;
  bool has_y = false;
  bool has_z = false;
  bool has_nx = false;
  bool has_ny = false;
  bool has_nz = false;
  bool has_texture_s = false;
  bool has_texture_t = false;
  bool has_texture_u = false;
  bool has_texture_v = false;
  bool has_s = false;
  bool has_t = false;
  bool has_u = false;
  bool has_v = false;
  bool has_vertex_indices = false;
  for (const auto& element : plyfile.get_elements()) {
    if (element.name == "vertex") {
      for (const auto& property : element.properties) {
        if (property.name == "x") {
          has_x = true;
        } else if (property.name == "y") {
          has_y = true;
        } else if (property.name == "z") {
          has_z = true;
        } else if (property.name == "nx") {
          has_nx = true;
        } else if (property.name == "ny") {
          has_ny = true;
        } else if (property.name == "nz") {
          has_nz = true;
        } else if (property.name == "texture_s") {
          has_texture_s = true;
        } else if (property.name == "texture_t") {
          has_texture_t = true;
        } else if (property.name == "texture_u") {
          has_texture_u = true;
        } else if (property.name == "texture_v") {
          has_texture_v = true;
        } else if (property.name == "s") {
          has_s = true;
        } else if (property.name == "t") {
          has_t = true;
        } else if (property.name == "u") {
          has_u = true;
        } else if (property.name == "v") {
          has_v = true;
        }
      }
    } else if (element.name == "face") {
      for (const auto& property : element.properties) {
        if (property.name == "vertex_indices") {
          has_vertex_indices = true;
        }
      }
    } else if (element.name == "tristrips") {
      std::cerr << "ERROR: PLY files containing triangle strips are unsupported"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  if (!has_x || !has_y || !has_z || !has_vertex_indices) {
    std::cerr << "ERROR: Invalid PLY file specified by trianglemesh parameter: "
                 "filename"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::shared_ptr<tinyply::PlyData> plyvertices =
      plyfile.request_properties_from_element("vertex", {"x", "y", "z"});
  std::shared_ptr<tinyply::PlyData> plyfaces =
      plyfile.request_properties_from_element("face", {"vertex_indices"});

  std::shared_ptr<tinyply::PlyData> plynormals;
  if (has_nx && has_ny && has_nz) {
    plynormals =
        plyfile.request_properties_from_element("vertex", {"nx", "ny", "nz"});
  }

  std::shared_ptr<tinyply::PlyData> plyuvs;
  if (has_texture_s && has_texture_t) {
    plyuvs = plyfile.request_properties_from_element(
        "vertex", {"texture_s", "texture_t"});
  } else if (has_texture_u && has_texture_v) {
    plyuvs = plyfile.request_properties_from_element(
        "vertex", {"texture_u", "texture_v"});
  } else if (has_s && has_t) {
    plyuvs = plyfile.request_properties_from_element("vertex", {"s", "t"});
  } else if (has_u && has_v) {
    plyuvs = plyfile.request_properties_from_element("vertex", {"u", "v"});
  }

  plyfile.read(file_stream);

  std::vector<Point> points;
  AddToVector3<geometric>(*plyvertices, points);

  std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> indices;
  AddToVector3<uint32_t>(*plyfaces, indices);

  std::vector<Vector> normals;
  AddToVector3<geometric>(*plynormals, normals);

  std::vector<std::pair<geometric, geometric>> uvs;
  AddToVector2<geometric>(*plyuvs, uvs);

  auto triangles = iris::geometry::AllocateTriangleMesh(
      points, indices, normals, uvs, material, material,
      front_emissive_material, back_emissive_material, normal_map, normal_map);

  return std::make_pair(std::move(triangles), Matrix::Identity());
}

};  // namespace

extern const std::unique_ptr<const ObjectBuilder<
    std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
    const ReferenceCounted<iris::Material>&,
    const ReferenceCounted<iris::NormalMap>&,
    const ReferenceCounted<EmissiveMaterial>&,
    const ReferenceCounted<EmissiveMaterial>&, const Matrix&>>
    g_trianglemesh_builder = std::make_unique<PlyMeshBuilder>();

}  // namespace iris::pbrt_frontend::shapes