#include "frontends/pbrt/textures/constant.h"

#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<ValueTexture2D<visual>> MakeConstant(
    const FloatTexture::Constant& constant, TextureManager& texture_manager) {
  return texture_manager.AllocateFloatTexture(constant.value());
}

ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> MakeConstant(
    const SpectrumTexture::Constant& constant,
    TextureManager& texture_manager) {
  return texture_manager.AllocateReflectorTexture(constant.value());
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
