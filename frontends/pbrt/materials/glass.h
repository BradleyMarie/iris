#ifndef _FRONTENDS_PBRT_MATERIALS_GLASS_
#define _FRONTENDS_PBRT_MATERIALS_GLASS_

#include <memory>
#include <tuple>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"

namespace iris::pbrt_frontend::materials {

extern const std::unique_ptr<const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>>
    g_glass_builder;

}  // namespace iris::pbrt_frontend::materials

#endif  // _FRONTENDS_PBRT_MATERIALS_GLASS_