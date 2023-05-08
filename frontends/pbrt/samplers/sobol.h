#ifndef _FRONTENDS_PBRT_SAMPLERS_SOBOL_
#define _FRONTENDS_PBRT_SAMPLERS_SOBOL_

#include <memory>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/samplers/result.h"

namespace iris::pbrt_frontend::samplers {

extern const std::unique_ptr<const ObjectBuilder<Result>> g_sobol_builder;

}  // namespace iris::pbrt_frontend::samplers

#endif  // _FRONTENDS_PBRT_SAMPLERS_SOBOL_