#include "iris/lights/point_light.h"

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

class PointLight final : public Light {
 public:
  PointLight(Point location, ReferenceCounted<Spectrum> spectrum) noexcept
      : spectrum_(std::move(spectrum)), location_(location) {}

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
  Point location_;
};

std::optional<Light::SampleResult> PointLight::Sample(
    const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
    SpectralAllocator& allocator) const {
  if (!spectrum_) {
    return std::nullopt;
  }

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

visual_t PointLight::Power(const PowerMatcher& power_matcher,
                           visual_t world_radius_squared) const {
  if (!spectrum_) {
    return static_cast<visual_t>(0.0);
  }

  return static_cast<visual_t>(4.0) * std::numbers::pi_v<visual_t> *
         power_matcher.Match(*spectrum_);
}

}  // namespace

ReferenceCounted<Light> MakePointLight(const Point& location,
                                       ReferenceCounted<Spectrum> spectrum) {
  if (!spectrum) {
    return ReferenceCounted<Light>();
  }

  return MakeReferenceCounted<PointLight>(location, std::move(spectrum));
}

}  // namespace lights
}  // namespace iris
