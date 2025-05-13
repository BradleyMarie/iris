#include "frontends/pbrt/area_lights/parse.h"

#include <utility>

#include "frontends/pbrt/area_lights/diffuse.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/emissive_material.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::AreaLightSource;

std::pair<ReferenceCounted<EmissiveMaterial>,
          ReferenceCounted<EmissiveMaterial>>
ParseAreaLightSource(const pbrt_proto::v3::AreaLightSource& area_light_source,
                     SpectrumManager& spectrum_manager) {
  std::pair<ReferenceCounted<EmissiveMaterial>,
            ReferenceCounted<EmissiveMaterial>>
      result;
  switch (area_light_source.area_light_source_type_case()) {
    case AreaLightSource::kDiffuse:
      result = area_lights::MakeDiffuse(area_light_source.diffuse(),
                                        spectrum_manager);
      break;
    case AreaLightSource::AREA_LIGHT_SOURCE_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
