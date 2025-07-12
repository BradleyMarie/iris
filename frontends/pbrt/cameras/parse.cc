#include "frontends/pbrt/cameras/parse.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/cameras/orthographic.h"
#include "frontends/pbrt/cameras/perspective.h"
#include "frontends/pbrt/matrix_manager.h"
#include "iris/camera.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::Camera;

std::function<std::unique_ptr<iris::Camera>(const std::pair<size_t, size_t>&)>
ParseCamera(const Camera& camera,
            const MatrixManager::Transformation& transformation) {
  std::function<std::unique_ptr<iris::Camera>(const std::pair<size_t, size_t>&)>
      result;
  switch (camera.camera_type_case()) {
    case Camera::kEnvironment:
      std::cerr << "ERROR: Unsupported Camera type: environment" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Camera::kOrthographic:
      result = cameras::MakeOrthographic(camera.orthographic(), transformation);
      break;
    case Camera::kPerspective:
      result = cameras::MakePerspective(camera.perspective(), transformation);
      break;
    case Camera::kRealistic:
      std::cerr << "ERROR: Unsupported Camera type: realistic" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Camera::CAMERA_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
