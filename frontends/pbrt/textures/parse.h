#ifndef _FRONTENDS_PBRT_TEXTURES_PARSE_
#define _FRONTENDS_PBRT_TEXTURES_PARSE_

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

void ParseFloatTexture(const pbrt_proto::v3::FloatTexture& float_texture,
                       ImageManager& image_manager,
                       TextureManager& texture_manager);

void ParseSpectrumTexture(
    const pbrt_proto::v3::SpectrumTexture& spectrum_texture,
    ImageManager& image_manager, TextureManager& texture_manager);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_TEXTURES_PARSE_
