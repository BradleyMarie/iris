#include "iris/integrators/internal/sample_direct_lighting.h"

#include <cassert>
#include <cmath>

namespace iris {
namespace integrators {
namespace internal {
namespace internal {

visual_t PowerHeuristic(visual_t sampled_pdf, visual_t computed_pdf) {
  assert(std::isfinite(sampled_pdf) && 0.0 <= sampled_pdf);
  assert(std::isfinite(computed_pdf) && 0.0 <= computed_pdf);

  sampled_pdf *= sampled_pdf;
  computed_pdf *= computed_pdf;

  return sampled_pdf / (sampled_pdf + computed_pdf);
}

const Spectrum* DeltaLight(const Light::SampleResult& sample,
                           const Ray& traced_ray,
                           const RayTracer::SurfaceIntersection intersection,
                           SpectralAllocator& allocator) {
  auto diffuse = intersection.bsdf.Reflectance(traced_ray.direction,
                                               sample.to_light, allocator);
  if (!diffuse) {
    return nullptr;
  }

  visual_t falloff =
      ClampedAbsDotProduct(intersection.shading_normal, sample.to_light);
  auto* spectrum = allocator.Scale(&sample.emission, falloff);
  return allocator.Reflect(spectrum, &diffuse->reflector);
}

const Spectrum* FromLightSample(
    const Light::SampleResult& sample, const Ray& traced_ray,
    const RayTracer::SurfaceIntersection intersection,
    VisibilityTester& visibility_tester, SpectralAllocator& allocator) {
  auto diffuse = intersection.bsdf.Reflectance(traced_ray.direction,
                                               sample.to_light, allocator);
  if (!diffuse) {
    return nullptr;
  }

  visual_t falloff =
      ClampedAbsDotProduct(intersection.shading_normal, sample.to_light);
  visual_t weight = PowerHeuristic(*sample.pdf, diffuse->pdf);
  visual_t attenuation = (falloff * weight) / *sample.pdf;
  auto* spectrum = allocator.Scale(&sample.emission, attenuation);
  return allocator.Reflect(spectrum, &diffuse->reflector);
}

const Spectrum* FromBsdfSample(
    const Bsdf::SampleResult& sample, const Light& light, const Ray& traced_ray,
    const RayTracer::SurfaceIntersection intersection,
    VisibilityTester& visibility_tester, SpectralAllocator& allocator) {
  visual_t light_pdf;
  auto emissions =
      light.Emission(intersection.hit_point.CreateRay(sample.direction),
                     visibility_tester, allocator, &light_pdf);
  if (!emissions || light_pdf <= 0.0) {
    return nullptr;
  }

  visual_t falloff =
      ClampedAbsDotProduct(intersection.shading_normal, sample.direction);
  visual_t weight = PowerHeuristic(sample.pdf, light_pdf);
  visual_t attenuation = (falloff * weight) / sample.pdf;
  auto* spectrum = allocator.Scale(emissions, attenuation);
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

  if (light_sample && !light_sample->pdf) {
    return internal::DeltaLight(*light_sample, traced_ray, intersection,
                                allocator);
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
  for (auto* light_samples =
           light_sampler.Sample(intersection.hit_point.ApproximateLocation());
       light_samples; light_samples = light_samples->next) {
    Sampler bsdf_sampler(rng);
    Sampler light_sampler(rng);

    if (light_samples->pdf && *light_samples->pdf <= 0.0) {
      continue;
    }

    auto* direct_light = EstimateDirectLighting(
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