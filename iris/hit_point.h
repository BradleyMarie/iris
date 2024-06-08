#ifndef _IRIS_HIT_POINT_
#define _IRIS_HIT_POINT_

#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {

class HitPoint {
 public:
  explicit HitPoint(const Point& hit_point,
                    const PositionError& hit_point_error,
                    const Vector& surface_normal) noexcept
      : hit_point_(hit_point),
        hit_point_error_(hit_point_error),
        surface_normal_(surface_normal) {}

  const Point& ApproximateLocation() const { return hit_point_; }

  Ray CreateRay(const Vector& direction) const;
  Ray CreateRayTo(const Point& point, geometric_t* distance = nullptr) const;

 private:
  Point hit_point_;
  PositionError hit_point_error_;
  Vector surface_normal_;
};

}  // namespace iris

#endif  // _IRIS_HIT_POINT_