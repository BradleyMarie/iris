#ifndef _IRIS_TEXTURES_FBM_TEXTURE_
#define _IRIS_TEXTURES_FBM_TEXTURE_

#include <cstdint>

#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace textures {

ReferenceCounted<FloatTexture> MakeFbmTexture(const Matrix& world_to_texture,
                                              uint8_t octaves,
                                              visual_t roughness);

ReferenceCounted<ReflectorTexture> MakeFbmTexture(
    const Matrix& world_to_texture, uint8_t octaves, visual_t roughness,
    ReferenceCounted<ReflectorTexture> reflectance);

}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_FBM_TEXTURE_
