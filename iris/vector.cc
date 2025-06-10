#include "iris/vector.h"

#include <cassert>
#include <cmath>

#include "iris/float.h"

namespace iris {

#ifndef NDEBUG

Vector::Vector(geometric x, geometric y, geometric z) noexcept
    : x(x), y(y), z(z) {
  // TODO: Assert Vector has length and/or add assertions for CoordinateSystem
  assert(std::isfinite(x));
  assert(std::isfinite(y));
  assert(std::isfinite(z));
}

const geometric& Vector::operator[](size_t index) const {
  assert(index < 3);
  const geometric* as_array = &x;
  return as_array[index];
}

#endif  // NDEBUG

}  // namespace iris
