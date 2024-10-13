#include "frontends/pbrt/spectrum_managers/internal/color_spectrum.h"

namespace iris {
namespace pbrt_frontend {
namespace spectrum_managers {
namespace internal {

visual_t ColorSpectrum::Intensity(visual_t wavelength) const {
  size_t index = static_cast<size_t>(wavelength);
  return values_[index];
}

}  // namespace internal
}  // namespace spectrum_managers
}  // namespace pbrt_frontend
}  // namespace iris