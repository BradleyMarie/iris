#ifndef _FRONTENDS_PBRT_TEXTURES_CONSTANT_
#define _FRONTENDS_PBRT_TEXTURES_CONSTANT_

#include <memory>
#include <string>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"

namespace iris::pbrt_frontend::textures {

extern const std::unique_ptr<const ObjectBuilder<
    void, TextureManager&, SpectrumManager&, const std::string&>>
    g_float_constant_builder;
extern const std::unique_ptr<const ObjectBuilder<
    void, TextureManager&, SpectrumManager&, const std::string&>>
    g_spectrum_constant_builder;

}  // namespace iris::pbrt_frontend::textures

#endif  // _FRONTENDS_PBRT_TEXTURES_CONSTANT_