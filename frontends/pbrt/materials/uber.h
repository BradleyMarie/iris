#ifndef _FRONTENDS_PBRT_MATERIALS_UBER_
#define _FRONTENDS_PBRT_MATERIALS_UBER_

#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

MaterialResult MakeUber(
    const pbrt_proto::v3::Material::Uber& uber,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    TextureManager& texture_manager);

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_UBER_
