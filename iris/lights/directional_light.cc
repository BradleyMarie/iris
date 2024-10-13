#define _USE_MATH_DEFINES
#include "iris/lights/directional_light.h"

#include <cmath>

namespace iris {
namespace lights {

std::optional<Light::SampleResult> DirectionalLight::Sample(
    const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  Ray trace_ray = hit_point.CreateRay(to_light_);
  if (!tester.Visible(trace_ray)) {
    return std::nullopt;
  }

  return Light::SampleResult{*spectrum_, trace_ray.direction, std::nullopt};
}

const Spectrum* DirectionalLight::Emission(const Ray& to_light,
                                           VisibilityTester& tester,
                                           SpectralAllocator& allocator,
                                           visual_t* pdf) const {
  return nullptr;
}

visual_t DirectionalLight::Power(const PowerMatcher& power_matcher,
                                 visual_t world_radius_squared) const {
  return static_cast<visual_t>(M_PI) * world_radius_squared *
         power_matcher.Match(*spectrum_);
}

}  // namespace lights
}  // namespace iris