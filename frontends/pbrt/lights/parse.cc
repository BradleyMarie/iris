#include "frontends/pbrt/lights/parse.h"

#include <variant>

#include "frontends/pbrt/lights/distant.h"
#include "frontends/pbrt/lights/infinite.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/environmental_light.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::LightSource;

std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
ParseLightSource(const LightSource& light_source,
                 const std::filesystem::path& search_root,
                 const Matrix& model_to_world,
                 SpectrumManager& spectrum_manager) {
  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
      result;
  switch (light_source.light_source_type_case()) {
    case LightSource::kDistant:
      result = lights::MakeDistant(light_source.distant(), model_to_world,
                                   spectrum_manager);
      break;
    case LightSource::kGoniometric:
      break;
    case LightSource::kInfinite:
      result = lights::MakeInfinite(light_source.infinite(), search_root,
                                    model_to_world, spectrum_manager);
      break;
    case LightSource::kPoint:
      break;
    case LightSource::kProjection:
      break;
    case LightSource::kSpot:
      break;
    case LightSource::LIGHT_SOURCE_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
