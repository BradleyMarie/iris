#include "frontends/pbrt/textures/fbm.h"

#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/textures/fbm_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

using ::iris::textures::MakeFbmTexture;
using ::pbrt_proto::v3::FloatTexture;
using ::pbrt_proto::v3::SpectrumTexture;

ReferenceCounted<iris::textures::FloatTexture> MakeFbm(
    const FloatTexture::FBm& wrinkled, TextureManager& texture_manager) {
  return MakeFbmTexture(static_cast<uint8_t>(wrinkled.octaves()),
                        static_cast<visual_t>(wrinkled.roughness()));
}

ReferenceCounted<iris::textures::ReflectorTexture> MakeFbm(
    const SpectrumTexture::FBm& wrinkled, TextureManager& texture_manager) {
  return MakeFbmTexture(
      texture_manager.AllocateReflectorTexture(static_cast<visual>(1.0)),
      static_cast<uint8_t>(wrinkled.octaves()),
      static_cast<visual_t>(wrinkled.roughness()));
}

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris
