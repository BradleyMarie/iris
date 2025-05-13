#ifndef _FRONTENDS_PBRT_AREA_LIGHTS_PARSE_
#define _FRONTENDS_PBRT_AREA_LIGHTS_PARSE_

#include <utility>

#include "frontends/pbrt/spectrum_manager.h"
#include "iris/emissive_material.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

std::pair<ReferenceCounted<EmissiveMaterial>,
          ReferenceCounted<EmissiveMaterial>>
ParseAreaLightSource(const pbrt_proto::v3::AreaLightSource& area_light_source,
                     SpectrumManager& spectrum_manager);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_AREA_LIGHTS_PARSE_
