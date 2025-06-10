#ifndef _IRIS_POINT_
#define _IRIS_POINT_

#include "iris/float.h"
#include "iris/vector.h"

namespace iris {

struct Point final {
#ifdef NDEBUG
  Point(geometric x, geometric y, geometric z) noexcept : x(x), y(y), z(z) {}
#else
  Point(geometric x, geometric y, geometric z) noexcept;
#endif  // NDEBUG

  Point(const Point&) noexcept = default;
  bool operator==(const Point&) const = default;

#ifdef NDEBUG
  const geometric& operator[](size_t index) const {
    const geometric* as_array = &x;
    return as_array[index];
  }
#else
  const geometric& operator[](size_t index) const;
#endif  // NDEBUG

  const geometric x;
  const geometric y;
  const geometric z;
};

static inline Vector operator-(const Point& minuend, const Point& subtrahend) {
  return Vector(minuend.x - subtrahend.x, minuend.y - subtrahend.y,
                minuend.z - subtrahend.z);
}

static inline Point operator-(const Point& minuend, const Vector& subtrahend) {
  return Point(minuend.x - subtrahend.x, minuend.y - subtrahend.y,
               minuend.z - subtrahend.z);
}

static inline Point operator+(const Point& addend0, const Vector& addend1) {
  return Point(addend0.x + addend1.x, addend0.y + addend1.y,
               addend0.z + addend1.z);
}

}  // namespace iris

#endif  // _IRIS_POINT_
