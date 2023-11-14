#include "frontends/pbrt/spectrum_managers/internal/color_reflector.h"

namespace iris::pbrt_frontend::spectrum_managers::internal {

visual_t ColorReflector::Reflectance(visual_t wavelength) const {
  size_t index = static_cast<size_t>(wavelength);
  return values_[index];
}

}  // namespace iris::pbrt_frontend::spectrum_managers::internal