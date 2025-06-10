#include "iris/position_error.h"

#include <cassert>
#include <cmath>
#include <limits>

#include "iris/float.h"

namespace iris {

#ifndef NDEBUG

PositionError::PositionError(geometric_t x, geometric_t y,
                             geometric_t z) noexcept
    : x(x), y(y), z(z) {
  assert(std::isfinite(x));
  assert(std::isfinite(y));
  assert(std::isfinite(z));
}

#endif  // NDEBUG

geometric_t RoundingError(unsigned number_of_rounds) {
  geometric_t machine_epsilon = std::numeric_limits<geometric_t>::epsilon() *
                                static_cast<geometric_t>(0.5);
  geometric_t rounding_error =
      static_cast<geometric_t>(number_of_rounds) * machine_epsilon;
  return rounding_error / (static_cast<geometric_t>(1.0) - rounding_error);
}

}  // namespace iris
