#ifndef _FRONTENDS_PBRT_TEXTURES_IMAGEMAP_
#define _FRONTENDS_PBRT_TEXTURES_IMAGEMAP_

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/reference_counted.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace textures {

ReferenceCounted<iris::textures::FloatTexture> MakeImageMap(
    const pbrt_proto::v3::FloatTexture::ImageMap& imagemap,
    ImageManager& image_manager, TextureManager& texture_manager);

ReferenceCounted<iris::textures::ReflectorTexture> MakeImageMap(
    const pbrt_proto::v3::SpectrumTexture::ImageMap& imagemap,
    ImageManager& image_manager, TextureManager& texture_manager);

}  // namespace textures
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_IMAGEMAP_
