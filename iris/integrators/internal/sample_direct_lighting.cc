#include "iris/integrators/internal/sample_direct_lighting.h"

#include <cmath>
#include <optional>

#include "iris/bsdf.h"
#include "iris/float.h"
#include "iris/light.h"
#include "iris/light_sampler.h"
#include "iris/random.h"
#include "iris/ray.h"
#include "iris/ray_tracer.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/spectrum.h"
#include "iris/vector.h"
#include "iris/visibility_tester.h"

namespace iris {
namespace integrators {
namespace internal {
namespace internal {

visual_t PowerHeuristic(visual_t sampled_pdf, visual_t computed_pdf) {
  sampled_pdf *= sampled_pdf;
  computed_pdf *= computed_pdf;
  return sampled_pdf / (sampled_pdf + computed_pdf);
}

const Spectrum* DeltaLight(const Light::SampleResult& sample,
                           const Ray& traced_ray,
                           const RayTracer::SurfaceIntersection intersection,
                           SpectralAllocator& allocator) {
  std::optional<Bsdf::ReflectanceResult> diffuse =
      intersection.bsdf.Reflectance(traced_ray.direction, sample.to_light,
                                    allocator);
  if (!diffuse) {
    return nullptr;
  }

  visual_t falloff =
      ClampedAbsDotProduct(intersection.shading_normal, sample.to_light);
  const Spectrum* spectrum = allocator.Scale(&sample.emission, falloff);
  return allocator.Reflect(spectrum, &diffuse->reflector);
}

const Spectrum* FromLightSample(
    const Light::SampleResult& sample, const Ray& traced_ray,
    const RayTracer::SurfaceIntersection intersection,
    VisibilityTester& visibility_tester, SpectralAllocator& allocator) {
  std::optional<Bsdf::ReflectanceResult> diffuse =
      intersection.bsdf.Reflectance(traced_ray.direction, sample.to_light,
                                    allocator);
  if (!diffuse) {
    return nullptr;
  }

  visual_t falloff =
      ClampedAbsDotProduct(intersection.shading_normal, sample.to_light);
  visual_t weight = PowerHeuristic(*sample.pdf, diffuse->pdf);
  visual_t attenuation = (falloff * weight) / *sample.pdf;
  const Spectrum* spectrum = allocator.Scale(&sample.emission, attenuation);
  return allocator.Reflect(spectrum, &diffuse->reflector);
}

const Spectrum* FromBsdfSample(
    const Bsdf::SampleResult& sample, const Light& light, const Ray& traced_ray,
    const RayTracer::SurfaceIntersection intersection,
    VisibilityTester& visibility_tester, SpectralAllocator& allocator) {
  visual_t light_pdf;
  const Spectrum* emissions =
      light.Emission(intersection.hit_point.CreateRay(sample.direction),
                     visibility_tester, allocator, &light_pdf);
  if (!emissions || !std::isfinite(light_pdf) || light_pdf <= 0.0) {
    return nullptr;
  }

  visual_t falloff =
      ClampedAbsDotProduct(intersection.shading_normal, sample.direction);
  visual_t weight = PowerHeuristic(sample.pdf, light_pdf);
  visual_t attenuation = (falloff * weight) / sample.pdf;
  const Spectrum* spectrum = allocator.Scale(emissions, attenuation);
  return allocator.Reflect(spectrum, &sample.reflector);
}

}  // namespace internal

const Spectrum* EstimateDirectLighting(
    const Light& light, const Ray& traced_ray,
    const RayTracer::SurfaceIntersection intersection, Sampler bsdf_sampler,
    Sampler light_sampler, VisibilityTester& visibility_tester,
    SpectralAllocator& allocator) {
  if (!intersection.bsdf.IsDiffuse()) {
    return nullptr;
  }

  std::optional<Light::SampleResult> light_sample =
      light.Sample(intersection.hit_point, std::move(light_sampler),
                   visibility_tester, allocator);
  if (light_sample) {
    if (!light_sample->pdf) {
      return internal::DeltaLight(*light_sample, traced_ray, intersection,
                                  allocator);
    } else if (std::isnan(*light_sample->pdf) ||
               *light_sample->pdf <= static_cast<visual_t>(0.0)) {
      light_sample = std::nullopt;
    }
  }

  const Spectrum* light_spectrum = nullptr;
  if (light_sample) {
    light_spectrum = internal::FromLightSample(
        *light_sample, traced_ray, intersection, visibility_tester, allocator);
  }

  std::optional<Bsdf::SampleResult> bsdf_sample = intersection.bsdf.Sample(
      traced_ray.direction, std::nullopt, std::move(bsdf_sampler), allocator,
      /*diffuse_only=*/true);

  const Spectrum* bsdf_spectrum = nullptr;
  if (bsdf_sample) {
    bsdf_spectrum =
        internal::FromBsdfSample(*bsdf_sample, light, traced_ray, intersection,
                                 visibility_tester, allocator);
  }

  return allocator.Add(light_spectrum, bsdf_spectrum);
}

const Spectrum* SampleDirectLighting(
    LightSampler& light_sampler, const Ray& traced_ray,
    const RayTracer::SurfaceIntersection intersection, Random& rng,
    VisibilityTester& visibility_tester, SpectralAllocator& allocator) {
  const Spectrum* result = nullptr;
  for (LightSample* light_samples =
           light_sampler.Sample(intersection.hit_point.ApproximateLocation());
       light_samples; light_samples = light_samples->next) {
    Sampler bsdf_sampler(rng);
    Sampler light_sampler(rng);

    if (light_samples->pdf && *light_samples->pdf <= 0.0) {
      continue;
    }

    const Spectrum* direct_light = EstimateDirectLighting(
        light_samples->light, traced_ray, intersection, std::move(bsdf_sampler),
        std::move(light_sampler), visibility_tester, allocator);

    if (light_samples->pdf) {
      direct_light = allocator.Scale(
          direct_light, static_cast<visual_t>(1.0) / *light_samples->pdf);
    }

    result = allocator.Add(result, direct_light);
  }
  return result;
}

}  // namespace internal
}  // namespace integrators
}  // namespace iris
