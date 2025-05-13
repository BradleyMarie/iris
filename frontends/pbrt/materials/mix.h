#ifndef _FRONTENDS_PBRT_MATERIALS_MIX_
#define _FRONTENDS_PBRT_MATERIALS_MIX_

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

MaterialResult MakeMix(
    const pbrt_proto::v3::Material::Mix& mix,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    const MaterialManager& material_manager, TextureManager& texture_manager);

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_MIX_
