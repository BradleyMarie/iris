#ifndef _FRONTENDS_PBRT_MATERIALS_DISNEY_
#define _FRONTENDS_PBRT_MATERIALS_DISNEY_

#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

MaterialResult MakeDisney(
    const pbrt_proto::v3::Material::Disney& disney,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    TextureManager& texture_manager);

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_DISNEY_
