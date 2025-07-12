#include "frontends/pbrt/shapes/parse.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <utility>
#include <vector>

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/parse.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/shapes/plymesh.h"
#include "frontends/pbrt/shapes/sphere.h"
#include "frontends/pbrt/shapes/trianglemesh.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::Shape;

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> ParseShape(
    const pbrt_proto::v3::Shape& shape, const Matrix& model_to_world,
    bool reverse_orientation, const pbrt_proto::v3::Material& material_proto,
    MaterialResult material,
    std::array<ReferenceCounted<EmissiveMaterial>, 2> emissive_materials,
    const std::filesystem::path& search_root,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) {
  if (shape.has_overrides()) {
    material =
        ParseMaterial(material_proto, shape.overrides(), search_root,
                      material_manager, texture_manager, spectrum_manager);
  }

  if (reverse_orientation) {
    std::swap(material.materials[0], material.materials[1]);
    std::swap(material.bumpmaps[0], material.bumpmaps[1]);
    std::swap(emissive_materials[0], emissive_materials[1]);
  }

  std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> result(
      {}, model_to_world);
  switch (shape.shape_type_case()) {
    case Shape::kCone:
      std::cerr << "ERROR: Unsupported Shape type: cone" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kCurve:
      std::cerr << "ERROR: Unsupported Shape type: curve" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kCylinder:
      std::cerr << "ERROR: Unsupported Shape type: cylinder" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kDisk:
      std::cerr << "ERROR: Unsupported Shape type: disk" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kHeightfield:
      std::cerr << "ERROR: Unsupported Shape type: heightfield" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kHyperboloid:
      std::cerr << "ERROR: Unsupported Shape type: hyperboloid" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kLoopsubdiv:
      std::cerr << "ERROR: Unsupported Shape type: loopsubdiv" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kNurbs:
      std::cerr << "ERROR: Unsupported Shape type: nurbs" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kParaboloid:
      std::cerr << "ERROR: Unsupported Shape type: paraboloid" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Shape::kPlymesh:
      return shapes::MakePlyMesh(shape.plymesh(), model_to_world,
                                 material.materials[0], material.materials[1],
                                 emissive_materials[0], emissive_materials[1],
                                 material.bumpmaps[0], material.bumpmaps[1],
                                 search_root, texture_manager);
    case Shape::kSphere:
      return shapes::MakeSphere(shape.sphere(), model_to_world,
                                material.materials[0], material.materials[1],
                                emissive_materials[0], emissive_materials[1],
                                material.bumpmaps[0], material.bumpmaps[1]);
    case Shape::kTrianglemesh:
      return shapes::MakeTriangleMesh(
          shape.trianglemesh(), model_to_world, material.materials[0],
          material.materials[1], emissive_materials[0], emissive_materials[1],
          material.bumpmaps[0], material.bumpmaps[1], texture_manager);
      break;
    case Shape::SHAPE_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
