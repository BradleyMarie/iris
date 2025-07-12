#include "iris/spectra/blackbody_spectrum.h"

#include <algorithm>
#include <cmath>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {
namespace {

class BlackbodySpectrum final : public Spectrum {
 public:
  BlackbodySpectrum(visual temperature_kelvin)
      : temperature_kelvin_(temperature_kelvin) {}

  visual_t Intensity(visual_t wavelength) const override;

 private:
  visual temperature_kelvin_;
};

visual_t BlackbodySpectrum::Intensity(visual_t wavelength) const {
  static const visual_t c = static_cast<visual>(299792458.0);
  static const visual_t h = static_cast<visual>(6.62606957e-34);
  static const visual_t kb = static_cast<visual>(1.3806488e-23);

  wavelength /= static_cast<visual_t>(1000000000.0);

  visual_t left_numerator = static_cast<visual_t>(2.0) * h * c * c;
  visual_t left_denominator =
      (wavelength * wavelength) * (wavelength * wavelength) * wavelength;
  visual_t right_denominator =
      std::exp((h * c) / (wavelength * kb * temperature_kelvin_)) -
      static_cast<visual_t>(1.0);

  visual_t result = left_numerator / (left_denominator * right_denominator);
  if (!std::isfinite(result)) {
    result = static_cast<visual_t>(0.0);
  }

  return std::max(static_cast<visual_t>(0.0), result);
}

}  // namespace

ReferenceCounted<Spectrum> MakeBlackbodySpectrum(visual temperature_kelvin) {
  if (!std::isfinite(temperature_kelvin) ||
      temperature_kelvin <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Spectrum>();
  }

  return MakeReferenceCounted<BlackbodySpectrum>(temperature_kelvin);
}

}  // namespace spectra
}  // namespace iris
