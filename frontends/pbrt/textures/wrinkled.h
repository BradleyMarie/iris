#ifndef _FRONTENDS_PBRT_TEXTURES_WRINKLED_
#define _FRONTENDS_PBRT_TEXTURES_WRINKLED_

#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

ReferenceCounted<iris::textures::FloatTexture> MakeWrinkled(
    const pbrt_proto::v3::FloatTexture::Wrinkled& wrinkled,
    TextureManager& texture_manager);

ReferenceCounted<iris::textures::ReflectorTexture> MakeWrinkled(
    const pbrt_proto::v3::SpectrumTexture::Wrinkled& wrinkled,
    TextureManager& texture_manager);

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_WRINKLED_
