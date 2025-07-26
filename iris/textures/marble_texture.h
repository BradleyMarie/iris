#ifndef _IRIS_TEXTURES_MARBLE_TEXTURE_
#define _IRIS_TEXTURES_MARBLE_TEXTURE_

#include <cstdint>

#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

ReferenceCounted<ReflectorTexture> MakeMarbleTexture(
    const Matrix& world_to_texture, uint8_t octaves, visual_t roughness,
    visual_t scale, visual_t variation, ReferenceCounted<ReflectorTexture> r,
    ReferenceCounted<ReflectorTexture> g, ReferenceCounted<ReflectorTexture> b);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_MARBLE_TEXTURE_
