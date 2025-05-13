#include "frontends/pbrt/shapes/parse.h"

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
    bool reverse_orientation,
    const std::pair<pbrt_proto::v3::Material, MaterialResult>& material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const std::filesystem::path& search_root,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) {
  MaterialResult actual_material = material.second;
  if (shape.has_overrides()) {
    actual_material =
        ParseMaterial(material.first, shape.overrides(), material_manager,
                      texture_manager, spectrum_manager);
  }

  const ReferenceCounted<Material>& front_material = actual_material.material;
  const ReferenceCounted<Material>& back_material = actual_material.material;
  const ReferenceCounted<EmissiveMaterial>& actual_front_emissive_material =
      reverse_orientation ? back_emissive_material : front_emissive_material;
  const ReferenceCounted<EmissiveMaterial>& actual_back_emissive_material =
      reverse_orientation ? front_emissive_material : back_emissive_material;
  const ReferenceCounted<NormalMap>& front_normal_map =
      reverse_orientation ? actual_material.bumpmaps[1]
                          : actual_material.bumpmaps[0];
  const ReferenceCounted<NormalMap>& back_normal_map =
      reverse_orientation ? actual_material.bumpmaps[0]
                          : actual_material.bumpmaps[1];

  std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> result(
      {}, model_to_world);
  switch (shape.shape_type_case()) {
    case Shape::kCone:
      break;
    case Shape::kCurve:
      break;
    case Shape::kCylinder:
      break;
    case Shape::kDisk:
      break;
    case Shape::kHeightfield:
      break;
    case Shape::kHyperboloid:
      break;
    case Shape::kLoopsubdiv:
      break;
    case Shape::kNurbs:
      break;
    case Shape::kParaboloid:
      break;
    case Shape::kPlymesh:
      return shapes::MakePlyMesh(
          shape.plymesh(), model_to_world, front_material, back_material,
          actual_front_emissive_material, actual_back_emissive_material,
          front_normal_map, back_normal_map, search_root, texture_manager);
    case Shape::kSphere:
      return shapes::MakeSphere(shape.sphere(), model_to_world, front_material,
                                back_material, actual_front_emissive_material,
                                actual_back_emissive_material, front_normal_map,
                                back_normal_map);
    case Shape::kTrianglemesh:
      return shapes::MakeTriangleMesh(
          shape.trianglemesh(), model_to_world, front_material, back_material,
          actual_front_emissive_material, actual_back_emissive_material,
          front_normal_map, back_normal_map, texture_manager);
      break;
    case Shape::SHAPE_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
