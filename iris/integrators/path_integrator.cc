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
#include "iris/reflectors/uniform_reflector.h"
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
using ::iris::reflectors::CreateUniformReflector;

static ReferenceCounted<Reflector> kPerfectReflector =
    CreateUniformReflector(static_cast<visual>(1.0));

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

  const Reflector* path_reflectance = kPerfectReflector.Get();
  visual_t path_attenuation = static_cast<visual_t>(1.0);
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
    if (!bsdf_sample->etendue_conservation_factor) {
      attenuation = ClampedAbsDotProduct(
          trace_result.surface_intersection->shading_normal,
          bsdf_sample->direction);
    } else {
      attenuation = *bsdf_sample->etendue_conservation_factor;
      add_light_emissions = true;
    }

    path_reflectance =
        spectral_allocator.Scale(path_reflectance, &bsdf_sample->reflector);
    path_attenuation /= bsdf_sample->pdf;
    attenuation /= bsdf_sample->pdf;

    if (min_bounces_ <= bounces) {
      std::optional<visual_t> roulette_pdf = russian_roulette_.Evaluate(
          rng, path_attenuation * albedo_matcher.Match(*path_reflectance));
      if (!roulette_pdf) {
        break;
      }

      path_attenuation /= *roulette_pdf;
      attenuation /= *roulette_pdf;
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
