#include "frontends/pbrt/cameras/orthographic.h"

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/matrix_manager.h"
#include "iris/camera.h"
#include "iris/cameras/orthographic_camera.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {

using ::iris::cameras::OrthographicCamera;
using ::pbrt_proto::CameraScreenWindow;

std::function<std::unique_ptr<iris::Camera>(const std::pair<size_t, size_t>&)>
MakeOrthographic(const pbrt_proto::OrthographicCamera& orthographic,
                 const MatrixManager::Transformation& transformation) {
  pbrt_proto::OrthographicCamera with_defaults =
      Defaults().cameras().orthographic();
  with_defaults.MergeFrom(orthographic);

  std::optional<geometric_t> aspect_ratio;
  if (with_defaults.has_frameaspectratio()) {
    if (with_defaults.frameaspectratio() <= 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: frameaspectratio"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    aspect_ratio = with_defaults.frameaspectratio();
  }

  std::optional<CameraScreenWindow> screenwindow;
  if (with_defaults.has_screenwindow()) {
    screenwindow = with_defaults.screenwindow();
  }

  return [aspect_ratio, screenwindow,
          transformation](const std::pair<size_t, size_t>& image_dimensions) {
    geometric_t actual_aspect_ratio = aspect_ratio.value_or(
        static_cast<geometric_t>(image_dimensions.second) /
        static_cast<geometric_t>(image_dimensions.first));

    std::array<geometric_t, 4> screen_bounds;
    if (actual_aspect_ratio > 1.0) {
      screen_bounds[0] = -actual_aspect_ratio;
      screen_bounds[1] = actual_aspect_ratio;
      screen_bounds[2] = -1.0;
      screen_bounds[3] = 1.0;
    } else {
      screen_bounds[0] = -1.0;
      screen_bounds[1] = 1.0;
      screen_bounds[2] = 1.0 / -actual_aspect_ratio;
      screen_bounds[3] = 1.0 / actual_aspect_ratio;
    }

    if (screenwindow) {
      screen_bounds[0] = screenwindow->x_min();
      screen_bounds[1] = screenwindow->x_max();
      screen_bounds[2] = screenwindow->y_min();
      screen_bounds[3] = screenwindow->y_max();
    }

    return std::make_unique<OrthographicCamera>(transformation.start,
                                                screen_bounds);
  };
}

}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris
