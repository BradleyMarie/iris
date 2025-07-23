#ifndef _IRIS_TEXTURES_INTERNAL_MATH_
#define _IRIS_TEXTURES_INTERNAL_MATH_

#include <algorithm>
#include <cstdint>

#include "iris/float.h"
#include "iris/point.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {
namespace textures {
namespace internal {

static inline Point ScalePoint(const Point& hit_point, geometric_t scalar) {
  return Point(hit_point.x * scalar, hit_point.y * scalar,
               hit_point.z * scalar);
}

static inline std::pair<Vector, Vector> GetDifferentials(
    const TextureCoordinates& coordinates) {
  if (coordinates.hit_point_differentials.has_value()) {
    return std::make_pair(
        Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
               static_cast<geometric>(0.0)),
        Vector(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
               static_cast<geometric>(0.0)));
  }

  return std::make_pair(coordinates.hit_point_differentials->dhit_point_dx,
                        coordinates.hit_point_differentials->dhit_point_dy);
}

visual_t PerlinNoise(const Point& hit_point);

visual_t FractionalBrownianMotion(const Point& hit_point,
                                  const Vector& dhit_point_dx,
                                  const Vector& dhit_point_dy, visual_t omega,
                                  uint8_t max_octaves);

visual_t Turbulence(const Point& hit_point, const Vector& dhit_point_dx,
                    const Vector& dhit_point_dy, visual_t omega,
                    uint8_t max_octaves);

}  // namespace internal
}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_INTERNAL_MATH_
