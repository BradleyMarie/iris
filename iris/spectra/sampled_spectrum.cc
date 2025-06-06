#include "iris/spectra/sampled_spectrum.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <vector>

#include "iris/float.h"
#include "iris/reference_counted.h"
#include "iris/spectrum.h"

namespace iris {
namespace spectra {
namespace {

class SampledSpectrum final : public Spectrum {
 public:
  SampledSpectrum(std::vector<visual> wavelengths,
                  std::vector<visual> intensitites)
      : wavelengths_(wavelengths), intensitites_(intensitites) {
    assert(!wavelengths_.empty() && !intensitites_.empty());
    assert(wavelengths_.size() == intensitites_.size());
    assert(std::is_sorted(wavelengths_.begin(), wavelengths_.end()));
  }

  visual_t Intensity(visual_t wavelength) const override;

 private:
  std::vector<visual> wavelengths_;
  std::vector<visual> intensitites_;
};

visual_t SampledSpectrum::Intensity(visual_t wavelength) const {
  if (wavelength <= wavelengths_.front()) {
    return intensitites_.front();
  }

  if (wavelength >= wavelengths_.back()) {
    return intensitites_.back();
  }

  auto iter =
      std::lower_bound(wavelengths_.begin(), wavelengths_.end(), wavelength);

  size_t next_index = iter - wavelengths_.begin();
  if (wavelengths_[next_index] == wavelength) {
    return intensitites_[next_index];
  }

  size_t index = next_index - 1;

  visual_t delta = wavelengths_[next_index] - wavelengths_[index];
  visual_t interpolation = (wavelength - wavelengths_[index]) / delta;
  visual_t result =
      std::lerp(intensitites_[index], intensitites_[next_index], interpolation);

  return std::max(static_cast<visual_t>(0.0), result);
}

}  // namespace

ReferenceCounted<Spectrum> MakeSampledSpectrum(
    const std::map<visual, visual>& samples) {
  std::vector<visual> wavelengths;
  std::vector<visual> intensitites;
  bool is_black = true;
  for (auto [wavelength, intensity] : samples) {
    if (!std::isfinite(wavelength) || !std::isfinite(intensity)) {
      continue;
    }

    if (intensity > static_cast<visual>(0.0)) {
      is_black = false;
    }

    wavelengths.push_back(wavelength);
    intensitites.push_back(std::max(static_cast<visual>(0.0), intensity));
  }

  if (wavelengths.empty() || is_black) {
    return ReferenceCounted<Spectrum>();
  }

  return MakeReferenceCounted<SampledSpectrum>(std::move(wavelengths),
                                               std::move(intensitites));
}

}  // namespace spectra
}  // namespace iris
