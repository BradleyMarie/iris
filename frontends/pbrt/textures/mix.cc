#include "frontends/pbrt/textures/mix.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/mixed_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeMixedTexture;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeMix(
    const FloatTexture::Mix& mix, TextureManager& texture_manager) {
  FloatTexture::Mix with_defaults = Defaults().float_textures().mix();
  with_defaults.MergeFrom(mix);

  return MakeMixedTexture(
      texture_manager.AllocateFloatTexture(with_defaults.tex1()),
      texture_manager.AllocateFloatTexture(with_defaults.tex2()),
      texture_manager.AllocateFloatTexture(with_defaults.amount()));
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeMix(
    const SpectrumTexture::Mix& mix, TextureManager& texture_manager) {
  SpectrumTexture::Mix with_defaults = Defaults().spectrum_textures().mix();
  with_defaults.MergeFrom(mix);

  return MakeMixedTexture(
      texture_manager.AllocateReflectorTexture(with_defaults.tex1()),
      texture_manager.AllocateReflectorTexture(with_defaults.tex2()),
      texture_manager.AllocateFloatTexture(with_defaults.amount()));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
