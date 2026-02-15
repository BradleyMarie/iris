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

  return [aspect_ratio,
          transformation](const std::pair<size_t, size_t>& image_dimensions) {
    geometric_t actual_aspect_ratio = aspect_ratio.value_or(
        static_cast<geometric_t>(image_dimensions.second) /
        static_cast<geometric_t>(image_dimensions.first));

    std::array<geometric_t, 2> half_frame_size;
    if (actual_aspect_ratio > 1.0) {
      half_frame_size[0] = static_cast<geometric_t>(actual_aspect_ratio);
      half_frame_size[1] = static_cast<geometric_t>(1.0);
    } else {
      half_frame_size[0] = static_cast<geometric_t>(1.0);
      half_frame_size[1] = static_cast<geometric_t>(1.0 / actual_aspect_ratio);
    }

    return std::make_unique<OrthographicCamera>(transformation.start,
                                                half_frame_size);
  };
}

}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris
