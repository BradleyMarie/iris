#include "iris/lights/spot_light.h"

#include <algorithm>
#include <numbers>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/hit_point.h"
#include "iris/light.h"
#include "iris/point.h"
#include "iris/power_matcher.h"
#include "iris/ray.h"
#include "iris/reference_counted.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/visibility_tester.h"

namespace iris {
namespace lights {
namespace {

class SpotLight final : public Light {
 public:
  SpotLight(const Point& location, const Vector& direction,
            geometric cutoff_angle, geometric falloff_start_angle,
            ReferenceCounted<Spectrum> spectrum) noexcept
      : location_(location),
        direction_(Normalize(direction)),
        cos_cutoff_angle_(std::cos(cutoff_angle)),
        cos_falloff_start_angle_(std::cos(falloff_start_angle)),
        spectrum_(std::move(spectrum)) {}

  std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
      SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Ray& to_light, VisibilityTester& tester,
                           SpectralAllocator& allocator,
                           visual_t* pdf) const override;

  visual_t Power(const PowerMatcher& power_matcher,
                 visual_t world_radius_squared) const override;

 private:
  Point location_;
  Vector direction_;
  geometric cos_cutoff_angle_;
  geometric cos_falloff_start_angle_;
  ReferenceCounted<Spectrum> spectrum_;
};

std::optional<Light::SampleResult> SpotLight::Sample(
    const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  if (!spectrum_) {
    return std::nullopt;
  }

  Vector to_hit_point = Normalize(hit_point.ApproximateLocation() - location_);

  geometric_t cos_theta = DotProduct(direction_, to_hit_point);
  if (cos_theta < cos_cutoff_angle_) {
    return std::nullopt;
  }

  geometric_t distance;
  Ray trace_ray = hit_point.CreateRayTo(location_, &distance);
  if (!tester.Visible(trace_ray, distance)) {
    return std::nullopt;
  }

  visual_t scalar = static_cast<visual_t>(1.0);
  if (cos_theta < cos_falloff_start_angle_) {
    visual_t delta = (cos_theta - cos_cutoff_angle_) /
                     (cos_falloff_start_angle_ - cos_cutoff_angle_);
    scalar = (delta * delta) * (delta * delta);
  }

  const Spectrum* result = allocator.Scale(spectrum_.Get(), scalar);
  if (!result) {
    return std::nullopt;
  }

  return Light::SampleResult{*result, trace_ray.direction, std::nullopt};
}

const Spectrum* SpotLight::Emission(const Ray& to_light,
                                    VisibilityTester& tester,
                                    SpectralAllocator& allocator,
                                    visual_t* pdf) const {
  return nullptr;
}

visual_t SpotLight::Power(const PowerMatcher& power_matcher,
                          visual_t world_radius_squared) const {
  if (!spectrum_) {
    return static_cast<visual_t>(0.0);
  }

  visual_t full_hemisphere_power = static_cast<visual_t>(2.0) *
                                   std::numbers::pi_v<visual_t> *
                                   power_matcher.Match(*spectrum_);

  visual_t power_at_cutoff =
      full_hemisphere_power * (static_cast<visual_t>(1.0) - cos_cutoff_angle_);
  visual_t power_at_falloff =
      full_hemisphere_power *
      (static_cast<visual_t>(1.0) - cos_falloff_start_angle_);

  visual_t power = power_at_cutoff;
  if (power_at_falloff < power_at_cutoff) {
    power += power_at_falloff;
    power *= static_cast<visual_t>(0.5);
  }

  return power;
}

}  // namespace

ReferenceCounted<Light> MakeSpotLight(const Point& location,
                                      const Vector& direction,
                                      geometric cutoff_angle,
                                      geometric falloff_start_angle,
                                      ReferenceCounted<Spectrum> spectrum) {
  if (!std::isfinite(cutoff_angle) ||
      cutoff_angle <= static_cast<geometric>(0.0) || !spectrum) {
    return ReferenceCounted<Light>();
  }

  constexpr geometric_t kToRadians =
      std::numbers::pi_v<geometric_t> / static_cast<geometric_t>(180.0);
  cutoff_angle =
      std::clamp(cutoff_angle * kToRadians, -std::numbers::pi_v<geometric_t>,
                 std::numbers::pi_v<geometric_t>);
  falloff_start_angle = std::clamp(falloff_start_angle * kToRadians,
                                   -std::numbers::pi_v<geometric_t>,
                                   std::numbers::pi_v<geometric_t>);

  return MakeReferenceCounted<SpotLight>(location, direction, cutoff_angle,
                                         falloff_start_angle,
                                         std::move(spectrum));
}

}  // namespace lights
}  // namespace iris
