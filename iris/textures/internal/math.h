#ifndef _IRIS_TEXTURES_INTERNAL_MATH_
#define _IRIS_TEXTURES_INTERNAL_MATH_

#include <algorithm>
#include <cstdint>

#include "iris/float.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {
namespace textures {
namespace internal {

visual_t PerlinNoise(const Point& hit_point);

visual_t Turbulence(const Point& hit_point, const Vector& dhit_point_dx,
                    const Vector& dhit_point_dy, visual_t omega,
                    uint8_t max_octaves);

}  // namespace internal
}  // namespace textures
}  // namespace iris

#endif  // _IRIS_TEXTURES_INTERNAL_MATH_
