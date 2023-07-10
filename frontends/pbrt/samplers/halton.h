#ifndef _FRONTENDS_PBRT_SAMPLERS_HALTON_
#define _FRONTENDS_PBRT_SAMPLERS_HALTON_

#include <memory>

#include "frontends/pbrt/object_builder.h"
#include "iris/image_sampler.h"

namespace iris::pbrt_frontend::samplers {

extern const std::unique_ptr<
    const ObjectBuilder<std::unique_ptr<iris::ImageSampler>>>
    g_halton_builder;

}  // namespace iris::pbrt_frontend::samplers

#endif  // _FRONTENDS_PBRT_SAMPLERS_HALTON_