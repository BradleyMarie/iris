#ifndef _FRONTENDS_PBRT_TEXTURES_IMAGEMAP_
#define _FRONTENDS_PBRT_TEXTURES_IMAGEMAP_

#include <memory>
#include <string>

#include "frontends/pbrt/image_manager.h"
#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/texture_manager.h"

namespace iris::pbrt_frontend::textures {

extern const std::unique_ptr<const ObjectBuilder<
    void, ImageManager&, TextureManager&, const std::string&>>
    g_float_imagemap_builder;
extern const std::unique_ptr<const ObjectBuilder<
    void, ImageManager&, TextureManager&, const std::string&>>
    g_spectrum_imagemap_builder;

}  // namespace iris::pbrt_frontend::textures

#endif  // _FRONTENDS_PBRT_TEXTURES_IMAGEMAP_