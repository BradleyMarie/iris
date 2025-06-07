#include "iris/environmental_lights/internal/distribution_1d.h"

#include <algorithm>
#include <cassert>

namespace iris {
namespace environmental_lights {
namespace internal {

Distribution1D::Distribution1D(std::span<const visual> values)
    : Distribution1D(std::vector<visual>(values.begin(), values.end())) {}

Distribution1D::Distribution1D(std::vector<visual> values)
    : values_(std::move(values)), cdf_(values_.size() + 1) {
  assert(!values_.empty());

  cdf_[0] = 0.0;
  for (size_t index = 0; index < values_.size(); index++) {
    visual_t scaled_value =
        values_[index] / static_cast<visual_t>(values_.size());
    cdf_[index + 1] =
        cdf_[index] + std::max(static_cast<visual>(0.0), scaled_value);
    assert(std::isfinite(values_[index]));
  }

  average_value_ = cdf_.back();

  if (cdf_.back() == static_cast<visual>(0.0)) {
    for (size_t index = 1; index < cdf_.size(); index++) {
      cdf_[index] =
          static_cast<visual_t>(index) / static_cast<visual_t>(values_.size());
    }
  } else {
    for (size_t index = 1; index < cdf_.size(); index++) {
      cdf_[index] /= cdf_.back();
    }
  }
}

geometric_t Distribution1D::SampleContinuous(Sampler& sampler, visual_t* pdf,
                                             size_t* offset) const {
  geometric_t sample = sampler.Next();
  auto iter =
      std::lower_bound(cdf_.begin() + 1, cdf_.end(),
                       static_cast<visual>(sample), std::less_equal<visual>());
  assert(iter != cdf_.end());

  size_t index = iter - cdf_.begin();
  geometric_t entry_offset =
      (sample - cdf_[index]) / (cdf_[index] - cdf_[index - 1]);

  if (pdf) {
    *pdf = average_value_ != static_cast<visual>(0.0)
               ? values_[index - 1] / average_value_
               : static_cast<visual_t>(0.0);
  }

  if (offset) {
    *offset = index - 1;
  }

  return (static_cast<geometric_t>(index) + entry_offset) /
         static_cast<geometric_t>(values_.size());
}

visual_t Distribution1D::PdfContinuous(geometric_t sample) const {
  assert(std::isfinite(sample) && sample >= 0.0 && sample < 1.0);

  size_t index = static_cast<geometric_t>(values_.size()) * sample;

  if (average_value_ == static_cast<visual>(0.0)) {
    return static_cast<visual_t>(0.0);
  }

  return values_[index] / average_value_;
}

size_t Distribution1D::SampleDiscrete(Sampler& sampler, visual_t* pdf) const {
  auto iter = std::lower_bound(cdf_.begin() + 1, cdf_.end(), sampler.Next(),
                               std::less_equal<visual>());
  assert(iter != cdf_.end());

  size_t index = iter - cdf_.begin() - 1u;

  if (pdf) {
    *pdf = PdfDiscrete(index);
  }

  return index;
}

visual_t Distribution1D::PdfDiscrete(size_t sample) const {
  assert(sample < values_.size());

  if (average_value_ == static_cast<visual>(0.0)) {
    return static_cast<visual_t>(0.0);
  }

  return values_[sample] /
         (average_value_ * static_cast<visual_t>(values_.size()));
}

}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris
