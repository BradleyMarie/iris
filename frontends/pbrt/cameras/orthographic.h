#ifndef _FRONTENDS_PBRT_CAMERAS_ORTHOGRAPHIC_
#define _FRONTENDS_PBRT_CAMERAS_ORTHOGRAPHIC_

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <utility>

#include "frontends/pbrt/matrix_manager.h"
#include "iris/camera.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {

std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>
MakeOrthographic(const pbrt_proto::OrthographicCamera& orthographic,
                 const MatrixManager::Transformation& transformation);

}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_CAMERAS_ORTHOGRAPHIC_
