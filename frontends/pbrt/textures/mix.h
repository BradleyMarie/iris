#ifndef _FRONTENDS_PBRT_TEXTURES_MIX_
#define _FRONTENDS_PBRT_TEXTURES_MIX_

#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

ReferenceCounted<iris::textures::FloatTexture> MakeMix(
    const pbrt_proto::MixFloatTexture& mix, TextureManager& texture_manager);

ReferenceCounted<iris::textures::ReflectorTexture> MakeMix(
    const pbrt_proto::MixSpectrumTexture& mix, TextureManager& texture_manager);

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_MIX_
