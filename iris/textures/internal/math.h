#ifndef _IRIS_TEXTURES_INTERNAL_MATH_
#define _IRIS_TEXTURES_INTERNAL_MATH_

#include <cstdint>

#include "iris/float.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {
namespace textures {
namespace internal {

visual_t PerlinNoise(const Point& p);

visual_t FractionalBrownianMotion(const Point& p, const Vector& dp_dx,
                                  const Vector& dp_dy, visual_t omega,
                                  uint8_t max_octaves);

visual_t Turbulence(const Point& p, const Vector& dp_dx, const Vector& dp_dy,
                    visual_t omega, uint8_t max_octaves);

}  // namespace internal
}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_INTERNAL_MATH_
