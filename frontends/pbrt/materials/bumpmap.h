#ifndef _FRONTENDS_PBRT_MATERIALS_BUMPMAP_
#define _FRONTENDS_PBRT_MATERIALS_BUMPMAP_

#include <utility>

#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "iris/textures/texture2d.h"

namespace iris::pbrt_frontend::materials {

std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>> MakeBumpMap(
    const ReferenceCounted<textures::ValueTexture2D<visual>>& texture);

}  // namespace iris::pbrt_frontend::materials

#endif  // _FRONTENDS_PBRT_MATERIALS_MATTE_