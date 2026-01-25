#include "frontends/pbrt/area_lights/diffuse.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <utility>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/emissive_material.h"
#include "iris/emissive_materials/constant_emissive_material.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace area_lights {

using ::iris::emissive_materials::MakeConstantEmissiveMaterial;
using ::pbrt_proto::v3::AreaLightSource;

std::array<ReferenceCounted<EmissiveMaterial>, 2> MakeDiffuse(
    const AreaLightSource::Diffuse& diffuse,
    SpectrumManager& spectrum_manager) {
  AreaLightSource::Diffuse with_defaults =
      Defaults().area_light_sources().diffuse();
  with_defaults.MergeFrom(diffuse);

  if (with_defaults.samples() > std::numeric_limits<uint8_t>::max()) {
    std::cerr << "ERROR: Out of range value for parameter: samples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::array<ReferenceCounted<EmissiveMaterial>, 2> result = {
      MakeConstantEmissiveMaterial(spectrum_manager.AllocateSpectrum(
          spectrum_manager.AllocateSpectrum(with_defaults.l()),
          spectrum_manager.AllocateSpectrum(with_defaults.scale()))),
  };

  if (with_defaults.twosided()) {
    result[1] = result[0];
  }

  return result;
}

}  // namespace area_lights
}  // namespace pbrt_frontend
}  // namespace iris
