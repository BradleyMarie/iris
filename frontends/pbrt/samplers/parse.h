#ifndef _FRONTENDS_PBRT_SAMPLERS_PARSE_
#define _FRONTENDS_PBRT_SAMPLERS_PARSE_

#include <memory>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/samplers/result.h"

namespace iris::pbrt_frontend::samplers {

const ObjectBuilder<Result>& Parse(Tokenizer& tokenizer);

const ObjectBuilder<Result>& Default();

}  // namespace iris::pbrt_frontend::samplers

#endif  // _FRONTENDS_PBRT_SAMPLERS_PARSE_