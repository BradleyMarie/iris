#ifndef _FRONTENDS_PBRT_SHAPES_PARSE_
#define _FRONTENDS_PBRT_SHAPES_PARSE_

#include <array>
#include <filesystem>
#include <utility>
#include <vector>

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> ParseShape(
    const pbrt_proto::v3::Shape& shape, const Matrix& model_to_world,
    bool reverse_orientation,
    const std::pair<pbrt_proto::v3::Material, MaterialResult>& material,
    const std::array<ReferenceCounted<EmissiveMaterial>, 2>& emissive_materials,
    const std::filesystem::path& search_root,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SHAPES_PARSE_
