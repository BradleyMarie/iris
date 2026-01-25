#ifndef _FRONTENDS_PBRT_MATERIALS_PARSE_
#define _FRONTENDS_PBRT_MATERIALS_PARSE_

#include <filesystem>

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

MaterialResult ParseMaterial(
    const pbrt_proto::v3::Material& material,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    const std::filesystem::path& search_root,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager);

MaterialResult ParseMakeNamedMaterial(
    const pbrt_proto::v3::MakeNamedMaterial& named_material,
    const std::filesystem::path& search_root, MaterialManager& material_manager,
    TextureManager& texture_manager, SpectrumManager& spectrum_manager);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_PARSE_
