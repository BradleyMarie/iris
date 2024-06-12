#ifndef _IRIS_INTERNAL_HIT_
#define _IRIS_INTERNAL_HIT_

#include <cmath>
#include <optional>

#include "iris/hit.h"
#include "iris/integer.h"
#include "iris/ray.h"

namespace iris {
class Geometry;
class Matrix;
namespace internal {

struct Hit final : public iris::Hit {
  Hit(iris::Hit* next, geometric_t distance, geometric_t distance_error,
      face_t front, face_t back, const void* additional_data) noexcept
      : iris::Hit{next, distance},
        distance_error(std::abs(distance_error)),
        geometry(nullptr),
        model_to_world(nullptr),
        front(front),
        back(back),
        additional_data(additional_data) {}

  geometric_t distance_error;
  const iris::Geometry* geometry;
  const iris::Matrix* model_to_world;
  face_t front;
  face_t back;
  const void* additional_data;
  std::optional<Ray> model_ray;
};

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_INTERNAL_HIT_