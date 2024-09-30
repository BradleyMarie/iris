#ifndef _FRONTENDS_PBRT_CAMERAS_PARSE_
#define _FRONTENDS_PBRT_CAMERAS_PARSE_

#include <functional>
#include <memory>

#include "frontends/pbrt/matrix_manager.h"
#include "frontends/pbrt/object_builder.h"
#include "iris/camera.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {

const ObjectBuilder<
    std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>,
    const MatrixManager::Transformation&>&
Parse(Tokenizer& tokenizer);

const ObjectBuilder<
    std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>,
    const MatrixManager::Transformation&>&
Default();

}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_CAMERAS_PARSE_