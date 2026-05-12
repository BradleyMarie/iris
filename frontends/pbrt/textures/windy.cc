#include "frontends/pbrt/textures/windy.h"

#include "frontends/pbrt/texture_manager.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/windy_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeWindyTexture;
using ::pbrt_proto::WindyFloatTexture;
using ::pbrt_proto::WindySpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeWindy(
    const WindyFloatTexture& windy, TextureManager& texture_manager,
    const Matrix& world_to_texture) {
  return MakeWindyTexture(world_to_texture);
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeWindy(
    const WindySpectrumTexture& windy, TextureManager& texture_manager,
    const Matrix& world_to_texture) {
  return MakeWindyTexture(
      world_to_texture,
      texture_manager.AllocateReflectorTexture(static_cast<visual>(1.0)));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
