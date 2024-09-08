#ifndef _FRONTENDS_PBRT_MATERIALS_MATERIAL_BUILDER_
#define _FRONTENDS_PBRT_MATERIALS_MATERIAL_BUILDER_

#include <memory>
#include <tuple>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"

namespace iris {
namespace pbrt_frontend {

class MaterialManager;

namespace materials {

typedef std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
    MaterialBuilderResult;

typedef ObjectBuilder<MaterialBuilderResult, const MaterialManager&,
                      TextureManager&>
    NestedMaterialBuilder;

typedef ObjectBuilder<std::shared_ptr<NestedMaterialBuilder>,
                      const MaterialManager&, TextureManager&>
    MaterialBuilder;

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_MATERIAL_BUILDER_