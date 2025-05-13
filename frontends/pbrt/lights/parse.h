#ifndef _FRONTENDS_PBRT_LIGHTS_PARSE_
#define _FRONTENDS_PBRT_LIGHTS_PARSE_

#include <filesystem>
#include <variant>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/environmental_light.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
ParseLightSource(const pbrt_proto::v3::LightSource& light_source,
                 const std::filesystem::path& search_root,
                 const Matrix& model_to_world,
                 SpectrumManager& spectrum_manager);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_LIGHTS_PARSE_
