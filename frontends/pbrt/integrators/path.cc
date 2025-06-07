#include "frontends/pbrt/integrators/path.h"

#include <cstdlib>
#include <iostream>
#include <limits>

#include "iris/integrators/path_integrator.h"
#include "iris/light_scene.h"
#include "iris/light_scenes/one_light_scene.h"
#include "iris/light_scenes/power_light_scene.h"

namespace iris {
namespace pbrt_frontend {
namespace integrators {

using ::iris::integrators::MakePathIntegrator;
using ::iris::light_scenes::OneLightScene;
using ::iris::light_scenes::PowerLightScene;
using ::pbrt_proto::v3::Integrator;

constexpr visual kMaximumContinueProbability = 0.95;
constexpr uint8_t kMinBounces = 3;

std::unique_ptr<IntegratorResult> MakePath(const Integrator::Path& path) {
  if (path.maxdepth() < 0 ||
      path.maxdepth() > std::numeric_limits<uint8_t>::max()) {
    std::cerr << "ERROR: Out of range value for parameter: maxdepth"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (path.rrthreshold() < 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: rrthreshold"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::optional<std::array<size_t, 4>> pixel_bounds;
  if (path.has_pixelbounds()) {
    if (path.pixelbounds().x_min() < 0 || path.pixelbounds().x_max() < 0 ||
        path.pixelbounds().y_min() < 0 || path.pixelbounds().y_max() < 0) {
      std::cerr << "ERROR: Negative value in parameter list: pixelbounds"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    if (path.pixelbounds().x_max() < path.pixelbounds().x_min() ||
        path.pixelbounds().y_max() < path.pixelbounds().y_min()) {
      std::cerr << "ERROR: Invalid values for parameter list: pixelbounds"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    pixel_bounds = std::array<size_t, 4>(
        {static_cast<size_t>(path.pixelbounds().x_min()),
         static_cast<size_t>(path.pixelbounds().x_max()),
         static_cast<size_t>(path.pixelbounds().y_min()),
         static_cast<size_t>(path.pixelbounds().y_max())});
  }

  std::unique_ptr<LightScene::Builder> light_scene_builder;
  switch (path.lightsamplestrategy()) {
    case Integrator::SPATIAL:
      // TODO: Implement
    case Integrator::POWER:
      light_scene_builder = std::make_unique<PowerLightScene::Builder>();
      break;
    case Integrator::UNIFORM:
      light_scene_builder = std::make_unique<OneLightScene::Builder>();
      break;
  }

  return std::make_unique<IntegratorResult>(IntegratorResult{
      MakePathIntegrator(kMaximumContinueProbability,
                         static_cast<visual>(path.rrthreshold()), kMinBounces,
                         static_cast<uint8_t>(path.maxdepth())),
      std::move(light_scene_builder), pixel_bounds});
}

}  // namespace integrators
}  // namespace pbrt_frontend
}  // namespace iris
