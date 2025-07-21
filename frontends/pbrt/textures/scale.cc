#include "frontends/pbrt/textures/scale.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/scaled_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeScaledTexture;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeScale(
    const FloatTexture::Scale& scale, TextureManager& texture_manager) {
  FloatTexture::Scale with_defaults = Defaults().float_textures().scale();
  with_defaults.MergeFrom(scale);

  return MakeScaledTexture(
      texture_manager.AllocateFloatTexture(with_defaults.tex1()),
      texture_manager.AllocateFloatTexture(with_defaults.tex2()));
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeScale(
    const SpectrumTexture::Scale& scale, TextureManager& texture_manager) {
  SpectrumTexture::Scale with_defaults = Defaults().spectrum_textures().scale();
  with_defaults.MergeFrom(scale);

  return MakeScaledTexture(
      texture_manager.AllocateReflectorTexture(with_defaults.tex1()),
      texture_manager.AllocateReflectorTexture(with_defaults.tex2()));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
