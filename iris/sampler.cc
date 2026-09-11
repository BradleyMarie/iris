#include "iris/sampler.h"

#undef NDEBUG  // Enable assertions at runtime
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numbers>
#include <utility>

#include "iris/float.h"

namespace iris {
namespace {

const static geometric_t kMinValue = std::nextafter(
    static_cast<geometric_t>(-1.0), static_cast<geometric_t>(1.0));
const static geometric_t kMaxValue = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(-1.0));

}  // namespace

visual_t Sampler::NextLinear1D() {
  assert(samples_1d_ > 0);

  samples_1d_ -= 1;

  visual_t next = rng_.NextVisual();
  assert(next >= static_cast<geometric_t>(0.0) &&
         next <= static_cast<geometric_t>(1.0));

  return next;
}

size_t Sampler::NextIndex1D(size_t max_value) {
  return rng_.NextIndex(max_value);
}

std::pair<geometric_t, geometric_t> Sampler::NextLinear2D() {
  assert(samples_2d_ > 0);

  samples_2d_ -= 1;

  visual_t next0 = rng_.NextGeometric();
  assert(next0 >= static_cast<geometric_t>(0.0) &&
         next0 <= static_cast<geometric_t>(1.0));

  visual_t next1 = rng_.NextGeometric();
  assert(next1 >= static_cast<geometric_t>(0.0) &&
         next1 <= static_cast<geometric_t>(1.0));

  return {next0, next1};
}

std::pair<geometric_t, geometric_t> Sampler::NextPolar() {
  std::pair<geometric_t, geometric_t> sample = NextLinear2D();

  geometric_t u = static_cast<geometric_t>(2.0) * sample.first -
                  static_cast<geometric_t>(1.0);
  u = std::clamp(u, kMinValue, kMaxValue);

  geometric_t v = static_cast<geometric_t>(2.0) * sample.second -
                  static_cast<geometric_t>(1.0);
  v = std::clamp(v, kMinValue, kMaxValue);

  if (u == static_cast<geometric_t>(0.0) &&
      v == static_cast<geometric_t>(0.0)) {
    return {static_cast<visual_t>(0.0), static_cast<visual_t>(0.0)};
  }

  geometric_t theta, radius;
  if (std::abs(u) > std::abs(v)) {
    radius = u;
    theta = static_cast<geometric_t>(std::numbers::pi / 4.0) * (v / u);
  } else {
    radius = v;
    theta = static_cast<geometric_t>(std::numbers::pi / 2.0) -
            static_cast<geometric_t>(std::numbers::pi / 4.0) * (u / v);
  }

  return {radius, theta};
}

}  // namespace iris
