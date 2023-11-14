#ifndef _FRONTENDS_PBRT_TEXTURES_PARSE_
#define _FRONTENDS_PBRT_TEXTURES_PARSE_

#include <string>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"

namespace iris::pbrt_frontend::textures {

const ObjectBuilder<void, TextureManager&, SpectrumManager&,
                    const std::string&>&
Parse(Tokenizer& tokenizer, std::string& name);

}  // namespace iris::pbrt_frontend::textures

#endif  // _FRONTENDS_PBRT_TEXTURES_PARSE_