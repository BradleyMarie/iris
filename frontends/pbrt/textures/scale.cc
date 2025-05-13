#include "frontends/pbrt/textures/scale.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/scaled_texture.h"
#include "iris/textures/texture2d.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::PointerTexture2D;
using ::iris::textures::ScaledSpectralTexture2D;
using ::iris::textures::ScaledValueTexture2D;
using ::iris::textures::ValueTexture2D;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<ValueTexture2D<visual>> MakeScale(
    const FloatTexture::Scale& scale, TextureManager& texture_manager) {
  FloatTexture::Scale with_defaults = Defaults().float_textures().scale();
  with_defaults.MergeFrom(scale);

  return MakeScaledValueTexture2D<visual>(
      texture_manager.AllocateFloatTexture(with_defaults.tex1()),
      texture_manager.AllocateFloatTexture(with_defaults.tex2()));
}

ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> MakeScale(
    const SpectrumTexture::Scale& scale, TextureManager& texture_manager) {
  SpectrumTexture::Scale with_defaults = Defaults().spectrum_textures().scale();
  with_defaults.MergeFrom(scale);

  return MakeScaledSpectralTexture2D<Reflector>(
      texture_manager.AllocateReflectorTexture(with_defaults.tex1()),
      texture_manager.AllocateReflectorTexture(with_defaults.tex2()));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
