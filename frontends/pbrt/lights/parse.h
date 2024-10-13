#ifndef _FRONTENDS_PBRT_LIGHTS_PARSE_
#define _FRONTENDS_PBRT_LIGHTS_PARSE_

#include <variant>

#include "frontends/pbrt/object_builder.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "frontends/pbrt/tokenizer.h"
#include "iris/environmental_light.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

const ObjectBuilder<
    std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>,
    SpectrumManager&, const Matrix&>&
Parse(Tokenizer& tokenizer);

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_LIGHTS_PARSE_