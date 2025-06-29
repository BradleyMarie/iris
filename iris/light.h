#ifndef _IRIS_LIGHT_
#define _IRIS_LIGHT_

#include <optional>

#include "iris/float.h"
#include "iris/hit_point.h"
#include "iris/power_matcher.h"
#include "iris/ray.h"
#include "iris/reference_countable.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/vector.h"
#include "iris/visibility_tester.h"

namespace iris {

class Light : public ReferenceCountable {
 public:
  struct SampleResult {
    const Spectrum& emission;
    const Vector to_light;
    const std::optional<visual_t> pdf;
  };

  virtual std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler, VisibilityTester& tester,
      SpectralAllocator& allocator) const = 0;

  virtual const Spectrum* Emission(const Ray& to_light,
                                   VisibilityTester& tester,
                                   SpectralAllocator& allocator,
                                   visual_t* pdf = nullptr) const = 0;

  virtual visual_t Power(const PowerMatcher& power_matcher,
                         visual_t world_radius_squared) const = 0;

  virtual ~Light() {}
};

}  // namespace iris

#endif  // _IRIS_LIGHT_
