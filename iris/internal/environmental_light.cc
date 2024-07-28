#include "iris/internal/environmental_light.h"

namespace iris::internal {
namespace {

visual_t ComputePowerScalar(const BoundingBox& scene_bounds) {
  Vector radius =
      static_cast<geometric_t>(0.5) * (scene_bounds.upper - scene_bounds.lower);
  return DotProduct(radius, radius);
}

}  // namespace

EnvironmentalLight::EnvironmentalLight(const iris::EnvironmentalLight& light,
                                       const BoundingBox& scene_bounds) noexcept
    : light_(light), power_scalar_(ComputePowerScalar(scene_bounds)) {}

std::optional<Light::SampleResult> EnvironmentalLight::Sample(
    const HitPoint& hit_point, Sampler sampler, iris::VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  auto sample = light_.Sample(sampler, allocator);
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
  auto* emissions = light_.Emission(to_light.direction, allocator, pdf);
  if (!emissions) {
    return nullptr;
  }

  if (!tester.Visible(to_light)) {
    return nullptr;
  }

  return emissions;
}

visual_t EnvironmentalLight::Power(const PowerMatcher& power_matcher) const {
  return power_scalar_ * light_.UnitPower(power_matcher);
}

}  // namespace iris::internal