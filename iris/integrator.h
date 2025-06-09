#ifndef _IRIS_INTEGRATOR_
#define _IRIS_INTEGRATOR_

#include <memory>

#include "iris/albedo_matcher.h"
#include "iris/light_sampler.h"
#include "iris/random.h"
#include "iris/ray_differential.h"
#include "iris/ray_tracer.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/visibility_tester.h"

namespace iris {

class Integrator {
 public:
  virtual const Spectrum* Integrate(RayDifferential ray, RayTracer& ray_tracer,
                                    LightSampler& light_sampler,
                                    VisibilityTester& visibility_tester,
                                    const AlbedoMatcher& albedo_matcher,
                                    SpectralAllocator& spectral_allocator,
                                    Random& rng) = 0;

  virtual std::unique_ptr<Integrator> Duplicate() const = 0;

  virtual ~Integrator() {}
};

}  // namespace iris

#endif  // _IRIS_INTEGRATOR_
