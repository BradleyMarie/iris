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
  assert(std::isfinite(half_frame_size[0]) && half_frame_size[0] != 0.0);
  assert(std::isfinite(half_frame_size[1]) && half_frame_size[1] != 0.0);
  assert(std::isfinite(std::tan(half_fov)) && 0.0 < std::tan(half_fov));
}

Ray PerspectiveCamera::Compute(
    const std::array<geometric_t, 2>& image_uv,
    const std::array<geometric_t, 2>* lens_uv) const {
  Point origin(0.0, 0.0, 0.0);
  Vector direction(
      std::lerp(-half_frame_size_[0], half_frame_size_[0], image_uv[0]),
      std::lerp(half_frame_size_[1], -half_frame_size_[1], image_uv[1]),
      image_plane_distance_);
  Ray camera_ray(origin, direction);
  return Normalize(camera_to_world_.Multiply(camera_ray));
}

bool PerspectiveCamera::HasLens() const { return false; }

}  // namespace cameras
}  // namespace iris