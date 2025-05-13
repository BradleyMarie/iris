#include "frontends/pbrt/cameras/perspective.h"

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <numbers>
#include <optional>
#include <utility>

#include "frontends/pbrt/matrix_manager.h"
#include "iris/camera.h"
#include "iris/cameras/pinhole_camera.h"
#include "iris/cameras/thin_lens_camera.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {

using ::iris::cameras::PinholeCamera;
using ::iris::cameras::ThinLensCamera;
using ::pbrt_proto::v3::Camera;

std::function<std::unique_ptr<iris::Camera>(const std::pair<size_t, size_t>&)>
MakePerspective(const Camera::Perspective& perspective,
                const MatrixManager::Transformation& transformation) {
  if (perspective.focaldistance() <= 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: focaldistance"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (perspective.fov() <= 0.0 || perspective.fov() >= 180.0) {
    std::cerr << "ERROR: Out of range value for parameter: fov" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (perspective.lensradius() < 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: lensradius"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::optional<geometric_t> aspect_ratio;
  if (perspective.has_frameaspectratio()) {
    if (perspective.frameaspectratio() <= 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: frameaspectratio"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    aspect_ratio = perspective.frameaspectratio();
  }

  if (perspective.has_fov() && perspective.has_halffov()) {
    std::cerr << "ERROR: Cannot specify parameters together: fov, halffov"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  geometric_t half_fov;
  if (perspective.has_halffov()) {
    if (perspective.halffov() <= 0.0 || perspective.halffov() >= 90.0) {
      std::cerr << "ERROR: Out of range value for parameter: halffov"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    half_fov = static_cast<geometric_t>(perspective.halffov() *
                                        (std::numbers::pi / 180.0));
  } else {
    half_fov = static_cast<geometric_t>(perspective.fov() *
                                        (std::numbers::pi / 360.0));
  }

  return [aspect_ratio, half_fov, focus_distance = perspective.focaldistance(),
          lens_radius = perspective.lensradius(),
          transformation](const std::pair<size_t, size_t>& image_dimensions)
             -> std::unique_ptr<iris::Camera> {
    geometric_t actual_aspect_ratio = aspect_ratio.value_or(
        static_cast<intermediate_t>(image_dimensions.second) /
        static_cast<intermediate_t>(image_dimensions.first));

    std::array<geometric_t, 2> half_frame_size;
    if (actual_aspect_ratio > 1.0) {
      half_frame_size[0] = actual_aspect_ratio;
      half_frame_size[1] = 1.0;
    } else {
      half_frame_size[0] = 1.0;
      half_frame_size[1] = 1.0 / actual_aspect_ratio;
    }

    if (lens_radius == 0.0) {
      return std::make_unique<PinholeCamera>(transformation.start,
                                             half_frame_size, half_fov);
    }

    return std::make_unique<ThinLensCamera>(
        transformation.start, half_frame_size, half_fov,
        static_cast<geometric_t>(lens_radius),
        static_cast<geometric_t>(focus_distance));
  };
}

}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris
