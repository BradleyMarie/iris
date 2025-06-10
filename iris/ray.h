#ifndef _IRIS_RAY_
#define _IRIS_RAY_

#include <concepts>

#include "iris/point.h"
#include "iris/vector.h"

namespace iris {

struct Ray {
  Ray(const Point& origin, const Vector& direction) noexcept
      : origin(origin), direction(direction) {}

  Ray(const Ray&) noexcept = default;

  bool operator==(const Ray&) const = default;

  Point Endpoint(std::floating_point auto distance) const {
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
