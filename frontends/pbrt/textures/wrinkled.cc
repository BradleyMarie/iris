#include "frontends/pbrt/textures/wrinkled.h"

#include <algorithm>

#include "frontends/pbrt/texture_manager.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "iris/textures/wrinkled_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeWrinkledTexture;
using ::pbrt_proto::WrinkledFloatTexture;
using ::pbrt_proto::WrinkledSpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeWrinkled(
    const WrinkledFloatTexture& wrinkled, TextureManager& texture_manager,
    const Matrix& world_to_texture) {
  return MakeWrinkledTexture(world_to_texture,
                             std::min(255u, wrinkled.octaves()),
                             static_cast<visual_t>(wrinkled.roughness()));
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeWrinkled(
    const WrinkledSpectrumTexture& wrinkled, TextureManager& texture_manager,
    const Matrix& world_to_texture) {
  return MakeWrinkledTexture(
      world_to_texture, std::min(255u, wrinkled.octaves()),
      static_cast<visual_t>(wrinkled.roughness()),
      texture_manager.AllocateReflectorTexture(static_cast<visual>(1.0)));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
