#ifndef _IRIS_LIGHTS_POINT_LIGHT_
#define _IRIS_LIGHTS_POINT_LIGHT_

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

class PointLight final : public Light {
 public:
  PointLight(Point location, ReferenceCounted<Spectrum> spectrum) noexcept
      : spectrum_(std::move(spectrum)), location_(location) {
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
  const ReferenceCounted<Spectrum> spectrum_;
  const Point location_;
};

}  // namespace lights
}  // namespace iris

#endif  // _IRIS_LIGHTS_POINT_LIGHT_