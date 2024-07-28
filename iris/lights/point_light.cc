#include "iris/lights/point_light.h"

#include <limits>

namespace iris {
namespace lights {

std::optional<Light::SampleResult> PointLight::Sample(
    const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  geometric_t distance;
  Ray trace_ray = hit_point.CreateRayTo(location_, &distance);
  if (!tester.Visible(trace_ray, distance)) {
    return std::nullopt;
  }

  return Light::SampleResult{*spectrum_, trace_ray.direction, std::nullopt};
}

const Spectrum* PointLight::Emission(const Ray& to_light,
                                     VisibilityTester& tester,
                                     SpectralAllocator& allocator,
                                     visual_t* pdf) const {
  return nullptr;
}

visual_t PointLight::Power(const PowerMatcher& power_matcher) const {
  return power_matcher.Match(*spectrum_);
}

}  // namespace lights
}  // namespace iris