#ifndef _IRIS_TEXTURES_CONSTANT_TEXTURE_
#define _IRIS_TEXTURES_CONSTANT_TEXTURE_

#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

ReferenceCounted<FloatTexture> MakeConstantTexture(visual value);

ReferenceCounted<ReflectorTexture> MakeConstantTexture(
    ReferenceCounted<Reflector> reflectance);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_CONSTANT_TEXTURE_
