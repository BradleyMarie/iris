#ifndef _IRIS_LIGHTS_DIRECTIONAL_LIGHT_
#define _IRIS_LIGHTS_DIRECTIONAL_LIGHT_

#include <cassert>
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

class DirectionalLight final : public Light {
 public:
  DirectionalLight(Vector to_light,
                   iris::ReferenceCounted<Spectrum> spectrum) noexcept
      : spectrum_(std::move(spectrum)), to_light_(to_light) {
    assert(spectrum_);
  }

  std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
      SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Ray& to_light, VisibilityTester& tester,
                           SpectralAllocator& allocator,
                           visual_t* pdf) const override;

  visual_t Power(const PowerMatcher& power_matcher,
                 visual_t world_radius_squared) const override;

 private:
  const iris::ReferenceCounted<Spectrum> spectrum_;
  const Vector to_light_;
};

}  // namespace lights
}  // namespace iris

#endif  // _IRIS_LIGHTS_DIRECTIONAL_LIGHT_