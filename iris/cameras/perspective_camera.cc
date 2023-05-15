#include "iris/cameras/perspective_camera.h"

#include <cassert>
#include <cmath>

namespace iris {
namespace cameras {

PerspectiveCamera::PerspectiveCamera(
    const Matrix& camera_to_world,
    const std::array<geometric_t, 2>& half_frame_size,
    geometric_t half_fov) noexcept
    : camera_to_world_(camera_to_world),
      half_frame_size_(half_frame_size),
      image_plane_distance_(std::min(half_frame_size[0], half_frame_size[1]) /
                            std::tan(half_fov)) {
  assert(std::isfinite(half_frame_size[0]) && 0.0 < half_frame_size[0]);
  assert(std::isfinite(half_frame_size[1]) && 0.0 < half_frame_size[1]);
  assert(std::isfinite(std::tan(half_fov)) && 0.0 < std::tan(half_fov));
}

RayDifferential PerspectiveCamera::Compute(
    const std::array<geometric_t, 2>& image_uv,
    const std::array<geometric_t, 2>& image_uv_dxdy,
    const std::array<geometric_t, 2>* lens_uv) const {
  Point origin(0.0, 0.0, 0.0);
  Vector base_direction(
      std::lerp(-half_frame_size_[0], half_frame_size_[0], image_uv[0]),
      std::lerp(half_frame_size_[1], -half_frame_size_[1], image_uv[1]),
      image_plane_distance_);
  Ray base = camera_to_world_.Multiply(Ray(origin, base_direction));

  Vector dx_direction(
      std::lerp(-half_frame_size_[0], half_frame_size_[0], image_uv_dxdy[0]),
      base_direction.y, image_plane_distance_);
  Ray dx = camera_to_world_.Multiply(Ray(origin, dx_direction));

  Vector dy_direction(
      base_direction.x,
      std::lerp(half_frame_size_[1], -half_frame_size_[1], image_uv_dxdy[1]),
      image_plane_distance_);
  Ray dy = camera_to_world_.Multiply(Ray(origin, dy_direction));

  return Normalize(RayDifferential(base, dx, dy));
}

bool PerspectiveCamera::HasLens() const { return false; }

}  // namespace cameras
}  // namespace iris