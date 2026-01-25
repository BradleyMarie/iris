#include "frontends/pbrt/lights/spot.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/light.h"
#include "iris/lights/spot_light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

using ::iris::lights::MakeSpotLight;
using ::pbrt_proto::v3::LightSource;

ReferenceCounted<Light> MakeSpot(const LightSource::Spot& spot,
                                 const Matrix& model_to_world,
                                 SpectrumManager& spectrum_manager) {
  LightSource::Spot with_defaults = Defaults().light_sources().spot();
  with_defaults.MergeFrom(spot);

  ReferenceCounted<Spectrum> i =
      spectrum_manager.AllocateSpectrum(with_defaults.i());
  if (!i) {
    return ReferenceCounted<Light>();
  }

  ReferenceCounted<Spectrum> scale =
      spectrum_manager.AllocateSpectrum(with_defaults.scale());
  if (!scale) {
    return ReferenceCounted<Light>();
  }

  ReferenceCounted<Spectrum> scaled =
      spectrum_manager.AllocateSpectrum(i, scale);
  if (!scaled) {
    return ReferenceCounted<Light>();
  }

  Point model_from(with_defaults.from().x(), with_defaults.from().y(),
                   with_defaults.from().z());
  Point model_to(with_defaults.to().x(), with_defaults.to().y(),
                 with_defaults.to().z());
  Point world_from = model_to_world.Multiply(model_from);
  Point world_to = model_to_world.Multiply(model_to);
  Vector world_direction = world_to - world_from;
  return MakeSpotLight(
      model_to_world.Multiply(model_from), world_direction,
      with_defaults.coneangle(),
      with_defaults.coneangle() - with_defaults.conedeltaangle(),
      std::move(scaled));
}

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
