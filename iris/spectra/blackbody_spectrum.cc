#include "iris/spectra/blackbody_spectrum.h"

#include <algorithm>
#include <cmath>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {
namespace {

static constexpr visual_t kWeinsConstant =
    static_cast<visual_t>(0.002897771955);

visual_t BlackbodyMeters(visual_t wavelength_meters,
                         visual_t temperature_kelvin) {
  static const visual_t c = static_cast<visual_t>(299792458.0);
  static const visual_t h = static_cast<visual_t>(6.62606957e-34);
  static const visual_t kb = static_cast<visual_t>(1.3806488e-23);

  visual_t left_numerator = static_cast<visual_t>(2.0) * h * c * c;
  visual_t left_denominator = (wavelength_meters * wavelength_meters) *
                              (wavelength_meters * wavelength_meters) *
                              wavelength_meters;
  visual_t right_denominator =
      std::exp((h * c) / (wavelength_meters * kb * temperature_kelvin)) -
      static_cast<visual_t>(1.0);

  visual_t result = left_numerator / (left_denominator * right_denominator);
  if (!std::isfinite(result)) {
    result = static_cast<visual_t>(0.0);
  }

  return std::max(static_cast<visual_t>(0.0), result);
}

visual_t BlackbodyNanometers(visual_t wavelength_nm,
                             visual_t temperature_kelvin) {
  return BlackbodyMeters(wavelength_nm / static_cast<visual_t>(1000000000.0),
                         temperature_kelvin);
}

class BlackbodySpectrum final : public Spectrum {
 public:
  BlackbodySpectrum(visual temperature_kelvin)
      : temperature_kelvin_(temperature_kelvin) {}

  visual_t Intensity(visual_t wavelength) const override;

 private:
  visual temperature_kelvin_;
};

visual_t BlackbodySpectrum::Intensity(visual_t wavelength) const {
  return BlackbodyNanometers(wavelength, temperature_kelvin_);
}

class ScaledBlackbodySpectrum final : public Spectrum {
 public:
  ScaledBlackbodySpectrum(visual temperature_kelvin, visual peak_intensity)
      : temperature_kelvin_(temperature_kelvin),
        scalar_(peak_intensity /
                BlackbodyMeters(kWeinsConstant / temperature_kelvin,
                                temperature_kelvin)) {}

  visual_t Intensity(visual_t wavelength) const override;

 private:
  visual temperature_kelvin_;
  visual_t scalar_;
};

visual_t ScaledBlackbodySpectrum::Intensity(visual_t wavelength) const {
  return BlackbodyNanometers(wavelength, temperature_kelvin_) * scalar_;
}

}  // namespace

ReferenceCounted<Spectrum> MakeBlackbodySpectrum(visual temperature_kelvin) {
  if (!std::isfinite(temperature_kelvin) ||
      temperature_kelvin <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Spectrum>();
  }

  return MakeReferenceCounted<BlackbodySpectrum>(temperature_kelvin);
}

ReferenceCounted<Spectrum> MakeScaledBlackbodySpectrum(
    visual temperature_kelvin, visual peak_intensity) {
  if (!std::isfinite(temperature_kelvin) ||
      temperature_kelvin <= static_cast<visual>(0.0) ||
      !std::isfinite(peak_intensity) ||
      peak_intensity <= static_cast<visual>(0.0)) {
    return ReferenceCounted<Spectrum>();
  }

  return MakeReferenceCounted<ScaledBlackbodySpectrum>(temperature_kelvin,
                                                       peak_intensity);
}

}  // namespace spectra
}  // namespace iris
