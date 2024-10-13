#include "iris/internal/environmental_light.h"

namespace iris::internal {

EnvironmentalLight::EnvironmentalLight(
    const iris::EnvironmentalLight& light) noexcept
    : light_(light) {}

std::optional<Light::SampleResult> EnvironmentalLight::Sample(
    const HitPoint& hit_point, Sampler sampler, iris::VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  std::optional<iris::EnvironmentalLight::SampleResult> sample =
      light_.Sample(sampler, allocator);
  if (!sample) {
    return std::nullopt;
  }

  if (!tester.Visible(hit_point.CreateRay(sample->to_light))) {
    return std::nullopt;
  }

  return Light::SampleResult{sample->emission, sample->to_light, sample->pdf};
}

const Spectrum* EnvironmentalLight::Emission(const Ray& to_light,
                                             iris::VisibilityTester& tester,
                                             SpectralAllocator& allocator,
                                             visual_t* pdf) const {
  const Spectrum* emissions =
      light_.Emission(to_light.direction, allocator, pdf);
  if (!emissions) {
    return nullptr;
  }

  if (!tester.Visible(to_light)) {
    return nullptr;
  }

  return emissions;
}

visual_t EnvironmentalLight::Power(const PowerMatcher& power_matcher,
                                   visual_t world_radius_squared) const {
  return light_.Power(power_matcher, world_radius_squared);
}

}  // namespace iris::internal