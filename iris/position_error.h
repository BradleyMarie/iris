#ifndef _IRIS_POSITION_ERROR_
#define _IRIS_POSITION_ERROR_

#include "iris/float.h"

namespace iris {

struct PositionError final {
#ifdef NDEBUG
  PositionError(geometric_t x, geometric_t y, geometric_t z) noexcept
      : x(x), y(y), z(z) {}
#else
  PositionError(geometric_t x, geometric_t y, geometric_t z) noexcept;
#endif  // NDEBUG

  PositionError(const PositionError&) noexcept = default;
  bool operator==(const PositionError&) const = default;

  const geometric_t x;  // May be negative
  const geometric_t y;  // May be negative
  const geometric_t z;  // May be negative
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

geometric_t RoundingError(unsigned number_of_rounds);

}  // namespace iris

#endif  // _IRIS_POSITION_ERROR_
