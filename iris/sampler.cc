#include "iris/sampler.h"

#include <cmath>

namespace iris {

std::optional<geometric_t> Sampler::NextImpl() {
  if (next_) {
    auto value = *next_;
    next_.reset();
    return value;
  }

  if (samples_ == 0) {
    return std::nullopt;
  }

  samples_ -= 1;

  return rng_.NextGeometric();
}

size_t Sampler::NextIndex(size_t max) {
  auto scaled_sample = NextImpl().value() * static_cast<geometric_t>(max);
  geometric_t floor = std::floor(scaled_sample);
  next_ = scaled_sample - floor;
  return floor;
}

geometric_t Sampler::Next() { return NextImpl().value(); }

}  // namespace iris