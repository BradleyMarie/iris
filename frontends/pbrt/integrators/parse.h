#ifndef _FRONTENDS_PBRT_INTEGRATORS_PARSE_
#define _FRONTENDS_PBRT_INTEGRATORS_PARSE_

#include <memory>

#include "frontends/pbrt/integrators/result.h"
#include "frontends/pbrt/object_builder.h"

namespace iris::pbrt_frontend::integrators {

const ObjectBuilder<Result>& Parse(Tokenizer& tokenizer);

const ObjectBuilder<Result>& Default();

}  // namespace iris::pbrt_frontend::integrators

#endif  // _FRONTENDS_PBRT_INTEGRATORS_PARSE_