#ifndef _FRONTENDS_PBRT_MATERIALS_METAL_
#define _FRONTENDS_PBRT_MATERIALS_METAL_

#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

MaterialResult MakeMetal(
    const pbrt_proto::v3::Material::Metal& metal,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    TextureManager& texture_manager, SpectrumManager& spectrum_manager);

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_METAL_
