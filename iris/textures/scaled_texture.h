#ifndef _IRIS_TEXTURES_SCALED_TEXTURE_
#define _IRIS_TEXTURES_SCALED_TEXTURE_

#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

ReferenceCounted<FloatTexture> MakeScaledTexture(
    ReferenceCounted<FloatTexture> texture0,
    ReferenceCounted<FloatTexture> texture1);

ReferenceCounted<ReflectorTexture> MakeScaledTexture(
    ReferenceCounted<ReflectorTexture> texture0,
    ReferenceCounted<ReflectorTexture> texture1);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_SCALED_TEXTURE_
