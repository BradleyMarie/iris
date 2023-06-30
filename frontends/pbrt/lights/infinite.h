#ifndef _FRONTENDS_PBRT_LIGHTS_INFINITE_
#define _FRONTENDS_PBRT_LIGHTS_INFINITE_

#include <memory>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/environmental_light.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris::pbrt_frontend::lights {

extern const std::unique_ptr<const ObjectBuilder<
    std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>,
    SpectrumManager&, const Matrix&>>
    g_infinite_builder;

}  // namespace iris::pbrt_frontend::lights

#endif  // _FRONTENDS_PBRT_LIGHTS_INFINITE_