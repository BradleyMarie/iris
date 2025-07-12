#include "frontends/pbrt/lights/point.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/light.h"
#include "iris/lights/point_light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

using ::iris::lights::MakePointLight;
using ::pbrt_proto::v3::LightSource;

ReferenceCounted<Light> MakePoint(const LightSource::Point& point,
                                  const Matrix& model_to_world,
                                  SpectrumManager& spectrum_manager) {
  LightSource::Point with_defaults = Defaults().light_sources().point();
  with_defaults.MergeFrom(point);

  ReferenceCounted<Spectrum> l =
      spectrum_manager.AllocateSpectrum(with_defaults.l());
  if (!l) {
    return ReferenceCounted<Light>();
  }

  ReferenceCounted<Spectrum> scale =
      spectrum_manager.AllocateSpectrum(with_defaults.scale());
  if (!scale) {
    return ReferenceCounted<Light>();
  }

  ReferenceCounted<Spectrum> scaled =
      spectrum_manager.AllocateSpectrum(l, scale);
  if (!scaled) {
    return ReferenceCounted<Light>();
  }

  Point model_from(with_defaults.from().x(), with_defaults.from().y(),
                   with_defaults.from().z());
  return MakePointLight(model_to_world.Multiply(model_from), std::move(scaled));
}

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
