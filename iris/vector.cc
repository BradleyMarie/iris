#include "iris/vector.h"

#include <cassert>
#include <cmath>

#include "iris/float.h"

namespace iris {

#ifndef NDEBUG

Vector::Vector(geometric x, geometric y, geometric z) noexcept
    : x(x), y(y), z(z) {
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

geometric_t Vector::Length() const {
  return std::sqrt(DotProduct(*this, *this));
}

std::pair<Vector, Vector> CoordinateSystem(const Vector& vector) {
  assert(!vector.IsZero());

  geometric nx, ny, nz;
  if (std::abs(vector.x) > std::abs(vector.y)) {
    geometric_t length = std::sqrt(vector.x * vector.x + vector.z * vector.z);
    nx = -vector.z / length;
    ny = 0.0;
    nz = vector.x / length;
  } else {
    geometric_t length = std::sqrt(vector.y * vector.y + vector.z * vector.z);
    nx = 0.0;
    ny = vector.z / length;
    nz = -vector.y / length;
  }

  Vector v1(nx, ny, nz);
  Vector v2 = CrossProduct(vector, v1);

  return {v1, v2};
}

}  // namespace iris
