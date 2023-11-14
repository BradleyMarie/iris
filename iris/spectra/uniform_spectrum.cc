#include "iris/spectra/uniform_spectrum.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace iris::spectra {

UniformSpectrum::UniformSpectrum(visual_t value) : value_(value) {
  assert(std::isfinite(value));
  assert(value >= 0.0);
}

visual_t UniformSpectrum::Intensity(visual_t wavelength) const {
  return value_;
}

}  // namespace iris::spectra