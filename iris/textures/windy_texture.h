#ifndef _IRIS_TEXTURES_WINDY_TEXTURE_
#define _IRIS_TEXTURES_WINDY_TEXTURE_

#include <cstdint>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

ReferenceCounted<FloatTexture> MakeWindyTexture();

ReferenceCounted<ReflectorTexture> MakeWindyTexture(
    ReferenceCounted<ReflectorTexture> reflectance);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_WINDY_TEXTURE_
