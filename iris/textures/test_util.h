#ifndef _IRIS_TEXTURES_TEST_UTIL_
#define _IRIS_TEXTURES_TEST_UTIL_

#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

ReferenceCounted<FloatTexture> MakeZeroTexture();
ReferenceCounted<ReflectorTexture> MakeBlackTexture();

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_TEST_UTIL_
