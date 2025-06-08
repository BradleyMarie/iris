#include "iris/lights/directional_light.h"

#include <numbers>
#include <optional>

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

class DirectionalLight final : public Light {
 public:
  DirectionalLight(Vector to_light,
                   ReferenceCounted<Spectrum> spectrum) noexcept
      : spectrum_(std::move(spectrum)), to_light_(to_light) {}

  std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
      SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Ray& to_light, VisibilityTester& tester,
                           SpectralAllocator& allocator,
                           visual_t* pdf) const override;

  visual_t Power(const PowerMatcher& power_matcher,
                 visual_t world_radius_squared) const override;

 private:
  ReferenceCounted<Spectrum> spectrum_;
  Vector to_light_;
};

std::optional<Light::SampleResult> DirectionalLight::Sample(
    const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  if (!spectrum_) {
    return std::nullopt;
  }

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
  if (!spectrum_) {
    return static_cast<visual_t>(0.0);
  }

  return std::numbers::pi_v<visual_t> * world_radius_squared *
         power_matcher.Match(*spectrum_);
}

}  // namespace

ReferenceCounted<Light> MakeDirectionalLight(
    const Vector& to_light, ReferenceCounted<Spectrum> spectrum) {
  if (!spectrum) {
    return ReferenceCounted<Light>();
  }

  return MakeReferenceCounted<DirectionalLight>(to_light, std::move(spectrum));
}

}  // namespace lights
}  // namespace iris
