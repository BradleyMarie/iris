#ifndef _IRIS_CAMERA_THIN_LENS_CAMERA_
#define _IRIS_CAMERA_THIN_LENS_CAMERA_

#include <array>
#include <optional>

#include "iris/camera.h"
#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/ray_differential.h"
#include "iris/vector.h"

namespace iris {
namespace cameras {

class ThinLensCamera final : public Camera {
 public:
  ThinLensCamera(const Matrix& world_to_camera,
                 const std::array<geometric_t, 2>& half_frame_size,
                 geometric_t half_fov, geometric_t lens_radius,
                 geometric_t focus_distance) noexcept;

  RayDifferential Compute(
      const std::array<geometric_t, 2>& image_uv,
      const std::array<geometric_t, 2>& image_uv_dxdy,
      const std::optional<std::array<geometric_t, 2>>& lens_uv) const override;
  bool HasLens() const override;

 private:
  const Matrix world_to_camera_;
  const std::array<geometric_t, 2> half_frame_size_;
  const geometric_t image_plane_distance_;
  const geometric_t lens_radius_;
  const geometric_t focus_distance_;
};

}  // namespace cameras
}  // namespace iris

#endif  // _IRIS_CAMERA_THIN_LENS_CAMERA_