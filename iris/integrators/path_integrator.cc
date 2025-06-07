#include "iris/integrators/path_integrator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include "iris/albedo_matcher.h"
#include "iris/float.h"
#include "iris/integrator.h"
#include "iris/integrators/internal/path_builder.h"
#include "iris/integrators/internal/russian_roulette.h"
#include "iris/integrators/internal/sample_direct_lighting.h"
#include "iris/integrators/internal/sample_indirect_lighting.h"
#include "iris/light_sampler.h"
#include "iris/random.h"
#include "iris/ray_differential.h"
#include "iris/ray_tracer.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/visibility_tester.h"

namespace iris {
namespace integrators {
namespace {

using ::iris::integrators::internal::PathBuilder;
using ::iris::integrators::internal::RussianRoulette;
using ::iris::integrators::internal::SampleDirectLighting;
using ::iris::integrators::internal::SampleIndirectLighting;

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
  RussianRoulette russian_roulette_;
  uint8_t min_bounces_;
  uint8_t max_bounces_;
};

PathIntegrator::PathIntegrator(visual maximum_path_continue_probability,
                               visual always_continue_path_throughput,
                               uint8_t min_bounces,
                               uint8_t max_bounces) noexcept
    : russian_roulette_(maximum_path_continue_probability,
                        always_continue_path_throughput),
      min_bounces_(min_bounces),
      max_bounces_(max_bounces) {
  reflectors_.reserve(max_bounces);
  spectra_.reserve(max_bounces);
  attenuations_.reserve(max_bounces);
}

const Spectrum* PathIntegrator::Integrate(
    RayDifferential ray, RayTracer& ray_tracer, LightSampler& light_sampler,
    VisibilityTester& visibility_tester, const AlbedoMatcher& albedo_matcher,
    SpectralAllocator& spectral_allocator, Random& rng) {
  PathBuilder path_builder(reflectors_, spectra_, attenuations_);

  visual_t path_throughput = 1.0;
  bool add_light_emissions = true;
  for (uint8_t bounces = 0;; bounces++) {
    RayTracer::TraceResult trace_result = ray_tracer.Trace(ray);

    if (add_light_emissions) {
      path_builder.Add(trace_result.emission, spectral_allocator);
      add_light_emissions = false;
    }

    if (!trace_result.surface_intersection) {
      break;
    }

    const Spectrum* direct_lighting = SampleDirectLighting(
        light_sampler, ray, *trace_result.surface_intersection, rng,
        visibility_tester, spectral_allocator);
    path_builder.Add(direct_lighting, spectral_allocator);

    if (bounces == max_bounces_) {
      break;
    }

    std::optional<Bsdf::SampleResult> bsdf_sample =
        SampleIndirectLighting(*trace_result.surface_intersection,
                               iris::Sampler(rng), spectral_allocator,
                               /*modified*/ ray);
    if (!bsdf_sample) {
      break;
    }

    visual_t attenuation;
    if (bsdf_sample->diffuse) {
      attenuation = ClampedAbsDotProduct(
          trace_result.surface_intersection->shading_normal,
          bsdf_sample->direction);
    } else {
      attenuation = static_cast<visual_t>(1.0);
      add_light_emissions = true;
    }

    attenuation /= bsdf_sample->pdf;

    path_throughput *=
        albedo_matcher.Match(bsdf_sample->reflector) * attenuation;

    if (min_bounces_ <= bounces) {
      std::optional<visual_t> roulette_pdf =
          russian_roulette_.Evaluate(rng, path_throughput);
      if (!roulette_pdf) {
        break;
      }

      visual_t inverse_roulette_pdf =
          static_cast<visual_t>(1.0) / *roulette_pdf;
      attenuation *= inverse_roulette_pdf;
      path_throughput *= inverse_roulette_pdf;
    }

    path_builder.Bounce(&bsdf_sample->reflector, attenuation);
  }

  return path_builder.Build(spectral_allocator);
}

std::unique_ptr<Integrator> PathIntegrator::Duplicate() const {
  return std::make_unique<PathIntegrator>(*this);
}

}  // namespace

std::unique_ptr<Integrator> MakePathIntegrator(
    visual maximum_path_continue_probability,
    visual always_continue_path_throughput, uint8_t min_bounces,
    uint8_t max_bounces) {
  return std::make_unique<PathIntegrator>(maximum_path_continue_probability,
                                          always_continue_path_throughput,
                                          min_bounces, max_bounces);
}

}  // namespace integrators
}  // namespace iris
