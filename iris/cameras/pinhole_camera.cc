#include "iris/cameras/pinhole_camera.h"

#include <cassert>
#include <cmath>

namespace iris {
namespace cameras {

PinholeCamera::PinholeCamera(const Matrix& world_to_camera,
                             const std::array<geometric_t, 4>& frame_bounds,
                             geometric_t half_fov) noexcept
    : world_to_camera_(world_to_camera),
      frame_bounds_(frame_bounds),
      image_plane_distance_(static_cast<geometric_t>(1.0) /
                            std::tan(half_fov)) {
  assert(std::isfinite(frame_bounds[0]));
  assert(std::isfinite(frame_bounds[1]));
  assert(std::isfinite(frame_bounds[3]));
  assert(std::isfinite(frame_bounds[4]));
  assert(std::isfinite(std::tan(half_fov)) && 0.0 < std::tan(half_fov));
}

RayDifferential PinholeCamera::Compute(
    const std::array<geometric_t, 2>& image_uv,
    const std::array<geometric_t, 2>& image_uv_dxdy,
    const std::optional<std::array<geometric_t, 2>>& lens_uv) const {
  Point origin(0.0, 0.0, 0.0);
  Vector base_direction(
      std::lerp(frame_bounds_[0], frame_bounds_[1], image_uv[0]),
      std::lerp(frame_bounds_[3], frame_bounds_[2], image_uv[1]),
      image_plane_distance_);
  Ray base = world_to_camera_.InverseMultiply(Ray(origin, base_direction));

  Vector dx_direction(
      std::lerp(frame_bounds_[0], frame_bounds_[1], image_uv_dxdy[0]),
      base_direction.y, base_direction.z);
  Ray dx = world_to_camera_.InverseMultiply(Ray(origin, dx_direction));

  Vector dy_direction(
      base_direction.x,
      std::lerp(frame_bounds_[3], frame_bounds_[2], image_uv_dxdy[1]),
      base_direction.z);
  Ray dy = world_to_camera_.InverseMultiply(Ray(origin, dy_direction));

  return Normalize(RayDifferential(base, dx, dy));
}

bool PinholeCamera::HasLens() const { return false; }

}  // namespace cameras
}  // namespace iris
