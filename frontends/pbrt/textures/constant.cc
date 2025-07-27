#include "frontends/pbrt/textures/constant.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeConstant(
    const FloatTexture::Constant& constant, TextureManager& texture_manager) {
  return texture_manager.AllocateFloatTexture(constant.value());
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeConstant(
    const SpectrumTexture::Constant& constant,
    TextureManager& texture_manager) {
  SpectrumTexture::Constant with_defaults =
      Defaults().spectrum_textures().constant();
  with_defaults.MergeFrom(constant);

  return texture_manager.AllocateReflectorTexture(with_defaults.value());
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
