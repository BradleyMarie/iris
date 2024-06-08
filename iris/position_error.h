#ifndef _IRIS_POSITION_ERROR_
#define _IRIS_POSITION_ERROR_

#include <cassert>
#include <cmath>
#include <cstdint>

#include "iris/float.h"

namespace iris {

struct PositionError final {
  explicit PositionError(geometric_t x, geometric_t y, geometric_t z) noexcept
      : x(x), y(y), z(z) {
    assert(std::isfinite(x));
    assert(std::isfinite(y));
    assert(std::isfinite(z));
  }

  PositionError(const PositionError&) noexcept = default;

  bool operator==(const PositionError&) const = default;

  const geometric_t x;
  const geometric_t y;
  const geometric_t z;
};

static inline PositionError operator+(const PositionError& error0,
                                      const PositionError& error1) {
  return PositionError(error0.x + error1.x, error0.y + error1.y,
                       error0.z + error1.z);
}

static inline PositionError operator*(const PositionError& error,
                                      geometric_t scalar) {
  return PositionError(error.x * scalar, error.y * scalar, error.z * scalar);
}

static constexpr geometric_t RoundingError(uint8_t number_of_rounds) {
  static constexpr geometric_t machine_epsilon =
      std::numeric_limits<geometric_t>::epsilon() *
      static_cast<geometric_t>(0.5);
  const geometric_t rounding_error =
      static_cast<geometric_t>(number_of_rounds) * machine_epsilon;
  return rounding_error / (static_cast<geometric_t>(1.0) - rounding_error);
}

}  // namespace iris

#endif  // _IRIS_POSITION_ERROR_