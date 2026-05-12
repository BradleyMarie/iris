#ifndef _FRONTENDS_PBRT_TEXTURES_WINDY_
#define _FRONTENDS_PBRT_TEXTURES_WINDY_

#include "frontends/pbrt/texture_manager.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

ReferenceCounted<iris::textures::FloatTexture> MakeWindy(
    const pbrt_proto::WindyFloatTexture& wrinkled,
    TextureManager& texture_manager, const Matrix& world_to_texture);

ReferenceCounted<iris::textures::ReflectorTexture> MakeWindy(
    const pbrt_proto::WindySpectrumTexture& wrinkled,
    TextureManager& texture_manager, const Matrix& world_to_texture);

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_WINDY_
