#ifndef _FRONTENDS_PBRT_MATERIALS_PARSE_
#define _FRONTENDS_PBRT_MATERIALS_PARSE_

#include <memory>
#include <tuple>

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"

namespace iris::pbrt_frontend::materials {

const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>&
Parse(Tokenizer& tokenizer);

void ParseNamed(Tokenizer& tokenizer, MaterialManager& material_manager,
                SpectrumManager& spectrum_manager,
                TextureManager& texture_manager);

const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>&
Default();

}  // namespace iris::pbrt_frontend::materials

#endif  // _FRONTENDS_PBRT_MATERIALS_PARSE_