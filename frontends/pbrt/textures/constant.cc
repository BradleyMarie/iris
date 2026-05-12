#include "frontends/pbrt/textures/constant.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::pbrt_proto::ConstantFloatTexture;
using ::pbrt_proto::ConstantSpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeConstant(
    const ConstantFloatTexture& constant, TextureManager& texture_manager) {
  return texture_manager.AllocateFloatTexture(constant.value());
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeConstant(
    const ConstantSpectrumTexture& constant, TextureManager& texture_manager) {
  ConstantSpectrumTexture with_defaults =
      Defaults().spectrum_textures().constant();
  with_defaults.MergeFrom(constant);

  return texture_manager.AllocateReflectorTexture(with_defaults.value());
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
