#include "frontends/pbrt/textures/scale.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/scaled_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeScaledTexture;
using ::pbrt_proto::ScaleFloatTexture;
using ::pbrt_proto::ScaleSpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeScale(
    const ScaleFloatTexture& scale, TextureManager& texture_manager) {
  ScaleFloatTexture with_defaults = Defaults().float_textures().scale();
  with_defaults.MergeFrom(scale);

  return MakeScaledTexture(
      texture_manager.AllocateFloatTexture(with_defaults.tex1()),
      texture_manager.AllocateFloatTexture(with_defaults.tex2()));
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeScale(
    const ScaleSpectrumTexture& scale, TextureManager& texture_manager) {
  ScaleSpectrumTexture with_defaults = Defaults().spectrum_textures().scale();
  with_defaults.MergeFrom(scale);

  return MakeScaledTexture(
      texture_manager.AllocateReflectorTexture(with_defaults.tex1()),
      texture_manager.AllocateReflectorTexture(with_defaults.tex2()));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
