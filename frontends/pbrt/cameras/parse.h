#ifndef _FRONTENDS_PBRT_CAMERAS_PARSE_
#define _FRONTENDS_PBRT_CAMERAS_PARSE_

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <utility>

#include "frontends/pbrt/matrix_manager.h"
#include "iris/camera.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>
ParseCamera(const pbrt_proto::v3::Camera& camera,
            const MatrixManager::Transformation& transformation);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_CAMERAS_PARSE_
