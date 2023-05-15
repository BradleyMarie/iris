#include "iris/cameras/orthographic_camera.h"

#include <cassert>
#include <cmath>

namespace iris {
namespace cameras {

OrthographicCamera::OrthographicCamera(
    const Matrix& camera_to_world,
    const std::array<geometric_t, 2>& half_frame_size) noexcept
    : camera_to_world_(camera_to_world), half_frame_size_(half_frame_size) {
  assert(std::isfinite(half_frame_size[0]) && 0.0 < half_frame_size[0]);
  assert(std::isfinite(half_frame_size[1]) && 0.0 < half_frame_size[1]);
}

RayDifferential OrthographicCamera::Compute(
    const std::array<geometric_t, 2>& image_uv,
    const std::array<geometric_t, 2>& image_uv_dxdy,
    const std::optional<std::array<geometric_t, 2>>& lens_uv) const {
  Point base_origin(
      std::lerp(-half_frame_size_[0], half_frame_size_[0], image_uv[0]),
      std::lerp(half_frame_size_[1], -half_frame_size_[1], image_uv[1]), 0.0);
  Vector direction(0.0, 0.0, 1.0);
  Ray base = camera_to_world_.Multiply(Ray(base_origin, direction));

  Point dx_origin(
      std::lerp(-half_frame_size_[0], half_frame_size_[0], image_uv_dxdy[0]),
      base_origin.y, 0.0);
  Ray dx = camera_to_world_.Multiply(Ray(dx_origin, direction));

  Point dy_origin(
      base_origin.x,
      std::lerp(half_frame_size_[1], -half_frame_size_[1], image_uv_dxdy[1]),
      0.0);
  Ray dy = camera_to_world_.Multiply(Ray(dy_origin, direction));

  return Normalize(RayDifferential(base, dx, dy));
}

bool OrthographicCamera::HasLens() const { return false; }

}  // namespace cameras
}  // namespace iris