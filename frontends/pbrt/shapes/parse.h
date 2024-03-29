#ifndef _FRONTENDS_PBRT_SHAPES_PARSE_
#define _FRONTENDS_PBRT_SHAPES_PARSE_

#include <vector>

#include "frontends/pbrt/object_builder.h"
#include "iris/geometry.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris::pbrt_frontend::shapes {

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> Parse(
    Tokenizer& tokenizer, const std::filesystem::path& search_root,
    SpectrumManager& spectrum_manager, TextureManager& texture_manager,
    const std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>& material_builder,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const Matrix& model_to_world, bool reverse_orientation);

}  // namespace iris::pbrt_frontend::shapes

#endif  // _FRONTENDS_PBRT_SHAPES_PARSE_