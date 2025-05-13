#ifndef _FRONTENDS_PBRT_MATERIALS_GLASS_
#define _FRONTENDS_PBRT_MATERIALS_GLASS_

#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

MaterialResult MakeGlass(
    const pbrt_proto::v3::Material::Glass& glass,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    TextureManager& texture_manager);

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_GLASS_
