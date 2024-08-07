#ifndef _IRIS_INTERNAL_AREA_LIGHT_
#define _IRIS_INTERNAL_AREA_LIGHT_

#include <optional>

#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/hit_point.h"
#include "iris/integer.h"
#include "iris/light.h"
#include "iris/matrix.h"
#include "iris/power_matcher.h"
#include "iris/ray.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"

namespace iris::internal {

class AreaLight final : public Light {
 public:
  AreaLight(const Geometry& geometry, const Matrix* model_to_world,
            face_t face) noexcept;

  std::optional<SampleResult> Sample(
      const HitPoint& hit_point, Sampler sampler,
      iris::VisibilityTester& tester,
      SpectralAllocator& allocator) const override;

  const Spectrum* Emission(const Ray& to_light, iris::VisibilityTester& tester,
                           SpectralAllocator& allocator,
                           visual_t* pdf = nullptr) const override;

  visual_t Power(const PowerMatcher& power_matcher) const override;

 private:
  const Geometry& geometry_;
  const Matrix* model_to_world_;
  const face_t face_;
};

}  // namespace iris::internal

#endif  // _IRIS_INTERNAL_AREA_LIGHT_