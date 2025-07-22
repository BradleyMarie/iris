#ifndef _IRIS_TEXTURES_MIXED_TEXTURE_
#define _IRIS_TEXTURES_MIXED_TEXTURE_

#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

ReferenceCounted<FloatTexture> MakeMixedTexture(
    ReferenceCounted<FloatTexture> texture0,
    ReferenceCounted<FloatTexture> texture1,
    ReferenceCounted<FloatTexture> amount);

ReferenceCounted<ReflectorTexture> MakeMixedTexture(
    ReferenceCounted<ReflectorTexture> texture0,
    ReferenceCounted<ReflectorTexture> texture1,
    ReferenceCounted<FloatTexture> amount);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_SCALED_TEXTURE_
