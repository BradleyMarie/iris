#ifndef _IRIS_CAMERA_PERSPECTIVE_CAMERA_
#define _IRIS_CAMERA_PERSPECTIVE_CAMERA_

#include <array>

#include "iris/camera.h"
#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace cameras {

class PerspectiveCamera final : public Camera {
 public:
  PerspectiveCamera(const Matrix& camera_to_world,
                    const std::array<geometric_t, 2>& half_frame_size,
                    geometric_t half_fov) noexcept;

  Ray Compute(const std::array<geometric_t, 2>& image_uv,
              const std::array<geometric_t, 2>* lens_uv) const override;
  bool HasLens() const override;

 private:
  const Matrix camera_to_world_;
  const std::array<geometric_t, 2> half_frame_size_;
  const geometric_t image_plane_distance_;
};

}  // namespace cameras
}  // namespace iris

#endif  // _IRIS_CAMERA_PERSPECTIVE_CAMERA_