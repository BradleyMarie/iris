#include "frontends/pbrt/materials/bumpmap.h"

#include "iris/normal_maps/bump_normal_map.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/scaled_texture.h"

namespace iris::pbrt_frontend::materials {

std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>> MakeBumpMap(
    const ReferenceCounted<textures::ValueTexture2D<visual>>& texture) {
  auto negative_scale =
      MakeReferenceCounted<textures::ConstantValueTexture2D<visual>>(-1.0);
  auto scaled_bumpmap =
      MakeReferenceCounted<textures::ScaledValueTexture2D<visual>>(
          std::move(negative_scale), texture);
  return std::make_pair(
      iris::MakeReferenceCounted<normals::BumpNormalMap>(texture),
      iris::MakeReferenceCounted<normals::BumpNormalMap>(scaled_bumpmap));
}

}  // namespace iris::pbrt_frontend::materials