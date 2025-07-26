#ifndef _FRONTENDS_PBRT_TEXTURES_FBM_
#define _FRONTENDS_PBRT_TEXTURES_FBM_

#include "frontends/pbrt/texture_manager.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

ReferenceCounted<iris::textures::FloatTexture> MakeFbm(
    const pbrt_proto::v3::FloatTexture::FBm& wrinkled,
    TextureManager& texture_manager, const Matrix& world_to_texture);

ReferenceCounted<iris::textures::ReflectorTexture> MakeFbm(
    const pbrt_proto::v3::SpectrumTexture::FBm& wrinkled,
    TextureManager& texture_manager, const Matrix& world_to_texture);

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_FBM_
