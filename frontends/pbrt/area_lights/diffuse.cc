#include "frontends/pbrt/area_lights/diffuse.h"

#include <cstdlib>
#include <iostream>
#include <utility>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "iris/emissive_material.h"
#include "iris/emissive_materials/constant_emissive_material.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace area_lights {

using ::iris::emissive_materials::ConstantEmissiveMaterial;
using ::pbrt_proto::v3::AreaLightSource;

std::pair<ReferenceCounted<EmissiveMaterial>,
          ReferenceCounted<EmissiveMaterial>>
MakeDiffuse(const AreaLightSource::Diffuse& diffuse,
            SpectrumManager& spectrum_manager) {
  AreaLightSource::Diffuse with_defaults =
      Defaults().area_light_sources().diffuse();
  with_defaults.MergeFrom(diffuse);

  if (with_defaults.samples() <= 0 ||
      with_defaults.samples() > std::numeric_limits<uint8_t>::max()) {
    std::cerr << "ERROR: Out of range value for parameter: samples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<EmissiveMaterial> front_material =
      MakeReferenceCounted<ConstantEmissiveMaterial>(
          spectrum_manager.AllocateSpectrum(with_defaults.l()));

  ReferenceCounted<EmissiveMaterial> back_material;
  if (with_defaults.twosided()) {
    back_material = front_material;
  }

  return std::make_pair(std::move(front_material), std::move(back_material));
}

}  // namespace area_lights
}  // namespace pbrt_frontend
}  // namespace iris
