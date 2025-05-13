#include "frontends/pbrt/lights/distant.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/light.h"
#include "iris/lights/directional_light.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {

using ::iris::lights::DirectionalLight;
using ::pbrt_proto::v3::LightSource;

ReferenceCounted<Light> MakeDistant(const LightSource::Distant& distant,
                                    const Matrix& model_to_world,
                                    SpectrumManager& spectrum_manager) {
  LightSource::Distant with_defaults = Defaults().light_sources().distant();
  with_defaults.MergeFrom(distant);

  if (with_defaults.to().x() == with_defaults.from().x() &&
      with_defaults.to().y() == with_defaults.from().y() &&
      with_defaults.to().z() == with_defaults.from().z()) {
    std::cerr << "ERROR: from and to must not be the same point" << std::endl;
    exit(EXIT_FAILURE);
  }

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
  Point model_to(with_defaults.to().x(), with_defaults.to().y(),
                 with_defaults.to().z());
  Point world_from = model_to_world.Multiply(model_from);
  Point world_to = model_to_world.Multiply(model_to);
  Vector world_direction = world_from - world_to;

  return MakeReferenceCounted<DirectionalLight>(world_direction,
                                                std::move(scaled));
}

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris
