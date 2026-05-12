#include "frontends/pbrt/textures/fbm.h"

#include <algorithm>

#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/fbm_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeFbmTexture;
using ::pbrt_proto::FBmFloatTexture;
using ::pbrt_proto::FBmSpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeFbm(
    const FBmFloatTexture& fbm, TextureManager& texture_manager,
    const Matrix& world_to_texture) {
  return MakeFbmTexture(world_to_texture, std::min(255u, fbm.octaves()),
                        static_cast<visual_t>(fbm.roughness()));
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeFbm(
    const FBmSpectrumTexture& fbm, TextureManager& texture_manager,
    const Matrix& world_to_texture) {
  return MakeFbmTexture(
      world_to_texture, std::min(255u, fbm.octaves()),
      static_cast<visual_t>(fbm.roughness()),
      texture_manager.AllocateReflectorTexture(static_cast<visual>(1.0)));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
