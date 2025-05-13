#ifndef _FRONTENDS_PBRT_INTEGRATORS_RESULT_
#define _FRONTENDS_PBRT_INTEGRATORS_RESULT_

#include <array>
#include <memory>
#include <optional>

#include "iris/integrator.h"
#include "iris/light_scene.h"

namespace iris {
namespace pbrt_frontend {

struct IntegratorResult {
  std::unique_ptr<Integrator> integrator;
  std::unique_ptr<LightScene::Builder> light_scene_builder;
  std::optional<std::array<size_t, 4>> pixel_bounds;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_INTEGRATORS_RESULT_