#ifndef _FRONTENDS_PBRT_TEXTURES_CONSTANT_
#define _FRONTENDS_PBRT_TEXTURES_CONSTANT_

#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

ReferenceCounted<iris::textures::ValueTexture2D<visual>> MakeConstant(
    const pbrt_proto::v3::FloatTexture::Constant& constant,
    TextureManager& texture_manager);

ReferenceCounted<iris::textures::PointerTexture2D<Reflector, SpectralAllocator>>
MakeConstant(const pbrt_proto::v3::SpectrumTexture::Constant& constant,
             TextureManager& texture_manager);

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_CONSTANT_
