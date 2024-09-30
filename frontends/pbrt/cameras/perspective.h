#ifndef _FRONTENDS_PBRT_CAMERAS_PERSPECTIVE_
#define _FRONTENDS_PBRT_CAMERAS_PERSPECTIVE_

#include <memory>

#include "frontends/pbrt/matrix_manager.h"
#include "frontends/pbrt/object_builder.h"
#include "iris/camera.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {

extern const std::unique_ptr<const ObjectBuilder<
    std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>,
    const MatrixManager::Transformation&>>
    g_perspective_builder;

}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_CAMERAS_PERSPECTIVE_