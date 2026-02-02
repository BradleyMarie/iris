#ifndef _FRONTENDS_PBRT_MATERIALS_BUMPMAP_
#define _FRONTENDS_PBRT_MATERIALS_BUMPMAP_

#include <array>

#include "frontends/pbrt/texture_manager.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

std::array<ReferenceCounted<NormalMap>, 2> MakeBumpMap(
    const pbrt_proto::FloatTextureParameter& bumpmap,
    TextureManager& texture_manager);

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_MATTE_
