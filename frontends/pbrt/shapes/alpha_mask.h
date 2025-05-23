#ifndef _FRONTENDS_PBRT_SHAPES_ALPHA_MASK_
#define _FRONTENDS_PBRT_SHAPES_ALPHA_MASK_

#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

// If this function returns false, the shape is completely transparent.
bool MakeAlphaMask(TextureManager& texture_manager,
                   const pbrt_proto::v3::FloatTextureParameter& alpha,
                   ReferenceCounted<textures::ValueTexture2D<bool>>& result);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SHAPES_ALPHA_MASK_
