#ifndef _IRIS_INTEGRATORS_MOCK_INTEGRATOR_
#define _IRIS_INTEGRATORS_MOCK_INTEGRATOR_

#include <memory>
#include <vector>

#include "iris/integrator.h"
#include "iris/light_sampler.h"
#include "iris/random.h"
#include "iris/ray_differential.h"
#include "iris/ray_tracer.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/visibility_tester.h"

namespace iris {
namespace integrators {

class PathIntegrator final : public Integrator {
 public:
  PathIntegrator(visual maximum_path_continue_probability,
                 visual always_continue_path_throughput, uint8_t min_bounces,
                 uint8_t max_bounces) noexcept;

  virtual const Spectrum* Integrate(RayDifferential ray, RayTracer& ray_tracer,
                                    LightSampler& light_sampler,
                                    VisibilityTester& visibility_tester,
                                    const AlbedoMatcher& albedo_matcher,
                                    SpectralAllocator& spectral_allocator,
                                    Random& rng) override;

  virtual std::unique_ptr<Integrator> Duplicate() const override;

 private:
  std::vector<const Reflector*> reflectors_;
  std::vector<const Spectrum*> spectra_;
  std::vector<visual_t> attenuations_;
  visual maximum_path_continue_probability_;
  visual always_continue_path_throughput_;
  uint8_t min_bounces_;
  uint8_t max_bounces_;
};

}  // namespace integrators
}  // namespace iris

#endif  // _IRIS_INTEGRATORS_MOCK_INTEGRATOR_