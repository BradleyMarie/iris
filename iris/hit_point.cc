#include "iris/hit_point.h"

#include <cmath>
#include <limits>

namespace iris {
namespace {

geometric_t ComponentAbsDotProduct(const Vector& v0, const Vector& v1) {
  return std::abs(v0.x * v1.x) + std::abs(v0.y * v1.y) + std::abs(v0.z * v1.z);
}

geometric ComputeComponent(geometric value, geometric_t offset) {
  value += offset;
  return std::nextafter(
      value, std::copysign(std::numeric_limits<geometric>::infinity(), offset));
}

}  // namespace

Ray HitPoint::CreateRay(const Vector& direction) const {
  geometric_t offset_scalar =
      std::copysign(ComponentAbsDotProduct(hit_point_error_, surface_normal_),
                    DotProduct(surface_normal_, direction));
  Vector offset = surface_normal_ * offset_scalar;

  Point origin(ComputeComponent(hit_point_.x, offset.x),
               ComputeComponent(hit_point_.y, offset.y),
               ComputeComponent(hit_point_.z, offset.z));
  return Ray(origin, direction);
}

Ray HitPoint::CreateRayTo(const Point& point) const {
  return CreateRay(Normalize(point - hit_point_));
}

}  // namespace iris