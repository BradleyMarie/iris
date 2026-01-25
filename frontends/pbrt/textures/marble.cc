#include "frontends/pbrt/textures/marble.h"

#include "frontends/pbrt/texture_manager.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/textures/marble_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeMarbleTexture;
using ::pbrt_proto::v3::Spectrum;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<iris::textures::ReflectorTexture> MakeMarble(
    const SpectrumTexture::Marble& marble, TextureManager& texture_manager,
    const Matrix& world_to_texture) {
  Spectrum r;
  r.mutable_rgb_spectrum()->set_r(1.0);

  Spectrum g;
  g.mutable_rgb_spectrum()->set_g(1.0);

  Spectrum b;
  b.mutable_rgb_spectrum()->set_b(1.0);

  return MakeMarbleTexture(world_to_texture,
                           static_cast<uint8_t>(marble.octaves()),
                           static_cast<visual_t>(marble.roughness()),
                           static_cast<visual_t>(marble.scale()),
                           static_cast<visual_t>(marble.variation()),
                           texture_manager.AllocateReflectorTexture(r),
                           texture_manager.AllocateReflectorTexture(g),
                           texture_manager.AllocateReflectorTexture(b));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
