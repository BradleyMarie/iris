#ifndef _FRONTENDS_PBRT_TEXTURES_SCALE_
#define _FRONTENDS_PBRT_TEXTURES_SCALE_

#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

ReferenceCounted<iris::textures::FloatTexture> MakeScale(
    const pbrt_proto::v3::FloatTexture::Scale& scale,
    TextureManager& texture_manager);

ReferenceCounted<iris::textures::ReflectorTexture> MakeScale(
    const pbrt_proto::v3::SpectrumTexture::Scale& scale,
    TextureManager& texture_manager);

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_SCALE_
