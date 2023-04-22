#ifndef _FRONTENDS_PBRT_TEXTURES_PARSE_
#define _FRONTENDS_PBRT_TEXTURES_PARSE_

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/texture_manager.h"

namespace iris::pbrt_frontend::textures {

const ObjectBuilder<void, TextureManager&>& Parse(Tokenizer& tokenizer);

}  // namespace iris::pbrt_frontend::textures

#endif  // _FRONTENDS_PBRT_TEXTURES_PARSE_