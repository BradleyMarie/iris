#ifndef _FRONTENDS_PBRT_SHAPE_PLYMESH_
#define _FRONTENDS_PBRT_SHAPE_PLYMESH_

#include <filesystem>
#include <utility>
#include <vector>

#include "frontends/pbrt/texture_manager.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> MakePlyMesh(
    const pbrt_proto::v3::Shape::PlyMesh& plymesh, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map,
    const std::filesystem::path& search_root, TextureManager& texture_manager,
    bool reversed_orientation);

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SHAPE_PLYMESH_
