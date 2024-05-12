#ifndef _IRIS_GEOMETRY_INTERNAL_MATH_
#define _IRIS_GEOMETRY_INTERNAL_MATH_

#include <cstdint>
#include <limits>

#include "iris/float.h"

namespace iris {
namespace geometry {
namespace internal {

static inline constexpr geometric Gamma(uint8_t number_of_rounds) {
  static constexpr geometric machine_epsilon =
      std::numeric_limits<geometric>::epsilon() * static_cast<geometric>(0.5);
  geometric rounding_error =
      static_cast<geometric>(number_of_rounds) * machine_epsilon;
  return rounding_error / (static_cast<geometric>(1.0) - rounding_error);
}

}  // namespace internal
}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_INTERNAL_MATH_