#include "iris/reflectors/sampled_reflector.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

namespace iris {
namespace reflectors {
namespace {

class SampledReflector final : public Reflector {
 public:
  SampledReflector(std::vector<visual> wavelengths,
                   std::vector<visual> intensitites);

  visual_t Reflectance(visual_t wavelength) const override;

 private:
  std::vector<visual> wavelengths_;
  std::vector<visual> intensitites_;
};

SampledReflector::SampledReflector(std::vector<visual> wavelengths,
                                   std::vector<visual> intensitites)
    : wavelengths_(std::move(wavelengths)),
      intensitites_(std::move(intensitites)) {
  assert(!wavelengths_.empty());
  assert(std::is_sorted(wavelengths_.begin(), wavelengths_.end()));
  assert(std::none_of(wavelengths_.begin(), wavelengths_.end(),
                      [](visual value) { return !std::isfinite(value); }));
  assert(!intensitites_.empty());
  assert(std::none_of(intensitites_.begin(), intensitites_.end(),
                      [](visual value) {
                        return value < static_cast<visual>(0.0) ||
                               value > static_cast<visual>(1.0);
                      }));
  assert(intensitites_.size() == wavelengths_.size());
}

visual_t SampledReflector::Reflectance(visual_t wavelength) const {
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

  return std::clamp(result, static_cast<visual_t>(0.0),
                    static_cast<visual_t>(1.0));
}

}  // namespace

ReferenceCounted<Reflector> CreateSampledReflector(
    const std::map<visual, visual>& samples) {
  if (samples.empty()) {
    return ReferenceCounted<Reflector>();
  }

  std::vector<visual> wavelengths;
  std::vector<visual> intensitites;
  bool is_black = true;
  for (const auto& [wavelength, intensity] : samples) {
    if (!std::isfinite(wavelength)) {
      continue;
    }

    wavelengths.push_back(wavelength);
    intensitites.push_back(std::clamp(intensity, static_cast<visual>(0.0),
                                      static_cast<visual>(1.0)));
    if (intensitites.back() != static_cast<visual>(0.0)) {
      is_black = false;
    }
  }

  if (is_black) {
    return ReferenceCounted<Reflector>();
  }

  return iris::MakeReferenceCounted<SampledReflector>(std::move(wavelengths),
                                                      std::move(intensitites));
}

}  // namespace reflectors
}  // namespace iris