#include "iris/sampler.h"

#undef NDEBUG  // Enable assertions at runtime
#include <algorithm>
#include <cassert>
#include <cmath>
#include <optional>

#include "iris/float.h"

namespace iris {

std::optional<geometric_t> Sampler::NextImpl() {
  if (next_) {
    std::optional<geometric_t> value = *next_;
    next_.reset();
    return value;
  }

  if (samples_ == 0) {
    return std::nullopt;
  }

  samples_ -= 1;

  geometric_t next = rng_.NextGeometric();
  assert(next >= static_cast<geometric_t>(0.0) &&
         next <= static_cast<geometric_t>(1.0));

  return next;
}

size_t Sampler::NextIndex(size_t max) {
  std::optional<geometric_t> next = NextImpl();
  assert(next.has_value());

  geometric_t scaled_sample = *next * static_cast<geometric_t>(max);
  geometric_t floor = std::floor(scaled_sample);
  next_ = scaled_sample - floor;

  return std::min(max - 1, static_cast<size_t>(floor));
}

geometric_t Sampler::Next() {
  std::optional<geometric_t> next = NextImpl();
  assert(next.has_value());

  return *next;
}

}  // namespace iris
