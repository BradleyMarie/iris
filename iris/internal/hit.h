#ifndef _IRIS_INTERNAL_HIT_
#define _IRIS_INTERNAL_HIT_

#include <cmath>
#include <optional>

#include "iris/float.h"
#include "iris/hit.h"
#include "iris/integer.h"
#include "iris/ray.h"

namespace iris {
class Geometry;
class Matrix;
namespace internal {

struct Hit final : public iris::Hit {
  Hit(iris::Hit* next, geometric_t distance, geometric_t distance_error,
      const Geometry* geometry, face_t front, face_t back, bool allow_emissive,
      const void* additional_data) noexcept
      : iris::Hit(next, distance),
        error(std::abs(distance_error)),
        geometry(geometry),
        model_to_world(nullptr),
        front(front),
        back(back),
        allow_emissive(allow_emissive),
        additional_data(additional_data) {}

  const geometric_t error;
  const Geometry* geometry;
  const Matrix* model_to_world;
  face_t front;
  face_t back;
  bool allow_emissive;
  const void* additional_data;
  std::optional<Ray> model_ray;
};

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_INTERNAL_HIT_
