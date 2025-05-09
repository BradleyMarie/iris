#ifndef _IRIS_RAY_
#define _IRIS_RAY_

#include <cassert>
#include <cmath>
#include <type_traits>

#include "iris/float.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {

struct Ray {
  constexpr explicit Ray(const Point& origin, const Vector& direction) noexcept
      : origin(origin), direction(direction) {}

  Ray(const Ray&) noexcept = default;

  bool operator==(const Ray&) const = default;

  template <typename T>
  Point Endpoint(T distance) const
    requires std::is_floating_point<T>::value
  {
    assert(std::isfinite(distance));
    return origin + direction * distance;
  }

  const Point origin;
  const Vector direction;
};

static inline Ray Normalize(const Ray& ray) {
  return Ray(ray.origin, Normalize(ray.direction));
}

}  // namespace iris

#endif  // _IRIS_RAY_