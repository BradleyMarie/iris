#include "iris/cameras/thin_lens_camera.h"

#include <cassert>
#include <cmath>
#include <numbers>

namespace iris {
namespace cameras {
namespace {

const static geometric_t kMinValue = std::nextafter(
    static_cast<geometric_t>(-1.0), static_cast<geometric_t>(1.0));
const static geometric_t kMaxValue = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(-1.0));

Point SampleLens(const std::array<geometric_t, 2>& uv,
                 geometric_t lens_radius) {
  geometric_t u =
      static_cast<geometric_t>(2.0) * uv[0] - static_cast<geometric_t>(1.0);
  u = std::clamp(u, kMinValue, kMaxValue);

  geometric_t v =
      static_cast<geometric_t>(2.0) * uv[1] - static_cast<geometric_t>(1.0);
  v = std::clamp(v, kMinValue, kMaxValue);

  if (u == static_cast<geometric_t>(0.0) &&
      v == static_cast<geometric_t>(0.0)) {
    return Point(static_cast<geometric>(0.0), static_cast<geometric>(0.0),
                 static_cast<geometric>(0.0));
  }

  geometric_t theta;
  if (std::abs(u) > std::abs(v)) {
    theta = static_cast<geometric_t>(std::numbers::pi * 0.25) * (v / u);
    lens_radius *= u;
  } else {
    theta = static_cast<geometric_t>(std::numbers::pi * 0.5) -
            static_cast<geometric_t>(std::numbers::pi * 0.25) * (u / v);
    lens_radius *= v;
  }

  geometric_t x = lens_radius * std::cos(theta);
  geometric_t y = lens_radius * std::sin(theta);

  return Point(x, y, static_cast<geometric>(0.0));
}

Vector Direction(const Point& on_image_plane, const Point& on_lens,
                 geometric_t focal_length) {
  Vector to_lens_center(-on_image_plane.x, -on_image_plane.y,
                        -on_image_plane.z);
  Vector to_lens_center_normalized = Normalize(to_lens_center);

  geometric_t distance_to_focal_point =
      focal_length / to_lens_center_normalized.z;
  Vector to_focal_distance =
      to_lens_center_normalized * distance_to_focal_point;

  Point focal_point(to_focal_distance.x, to_focal_distance.y,
                    to_focal_distance.z);
  return focal_point - on_lens;
}

}  // namespace

ThinLensCamera::ThinLensCamera(
    const Matrix& world_to_camera,
    const std::array<geometric_t, 2>& half_frame_size, geometric_t half_fov,
    geometric_t lens_radius, geometric_t focus_distance) noexcept
    : world_to_camera_(world_to_camera),
      half_frame_size_(half_frame_size),
      image_plane_distance_(std::min(half_frame_size[0], half_frame_size[1]) /
                            std::tan(half_fov)),
      lens_radius_(lens_radius),
      focus_distance_(focus_distance) {
  assert(std::isfinite(half_frame_size[0]) && 0.0 < half_frame_size[0]);
  assert(std::isfinite(half_frame_size[1]) && 0.0 < half_frame_size[1]);
  assert(std::isfinite(std::tan(half_fov)) && 0.0 < std::tan(half_fov));
  assert(std::isfinite(lens_radius) && 0.0 < lens_radius);
  assert(std::isfinite(focus_distance) && 0.0 < focus_distance);
}

RayDifferential ThinLensCamera::Compute(
    const std::array<geometric_t, 2>& image_uv,
    const std::array<geometric_t, 2>& image_uv_dxdy,
    const std::optional<std::array<geometric_t, 2>>& lens_uv) const {
  Point origin = SampleLens(*lens_uv, lens_radius_);

  Point on_image_plane(
      std::lerp(-half_frame_size_[0], half_frame_size_[0], image_uv[0]),
      std::lerp(half_frame_size_[1], -half_frame_size_[1], image_uv[1]),
      image_plane_distance_);
  Ray base = world_to_camera_.InverseMultiply(
      Ray(origin, Direction(on_image_plane, origin, focus_distance_)));

  Point on_image_plane_dx(
      std::lerp(-half_frame_size_[0], half_frame_size_[0], image_uv_dxdy[0]),
      on_image_plane.y, on_image_plane.z);
  Ray dx = world_to_camera_.InverseMultiply(
      Ray(origin, Direction(on_image_plane_dx, origin, focus_distance_)));

  Point on_image_plane_dy(
      on_image_plane.x,
      std::lerp(half_frame_size_[1], -half_frame_size_[1], image_uv_dxdy[1]),
      on_image_plane.z);
  Ray dy = world_to_camera_.InverseMultiply(
      Ray(origin, Direction(on_image_plane_dy, origin, focus_distance_)));

  return Normalize(RayDifferential(base, dx, dy));
}

bool ThinLensCamera::HasLens() const { return true; }

}  // namespace cameras
}  // namespace iris