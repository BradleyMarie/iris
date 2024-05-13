#include "iris/integrators/internal/sample_indirect_lighting.h"

namespace iris {
namespace integrators {
namespace internal {

std::optional<Bsdf::Differentials> MakeDifferentials(
    const RayDifferential& trace_ray,
    const RayTracer::SurfaceIntersection& intersection) {
  if (!trace_ray.differentials || !intersection.differentials) {
    return std::nullopt;
  }

  return Bsdf::Differentials{trace_ray.differentials->dx.direction,
                             trace_ray.differentials->dy.direction};
}

std::optional<Bsdf::SampleResult> SampleIndirectLighting(
    const RayTracer::SurfaceIntersection& intersection, Sampler sampler,
    SpectralAllocator& allocator, RayDifferential& trace_ray) {
  auto differentials = MakeDifferentials(trace_ray, intersection);

  auto bsdf_sample = intersection.bsdf.Sample(
      trace_ray.direction, differentials, std::move(sampler), allocator);

  if (bsdf_sample) {
    if (differentials && bsdf_sample->differentials) {
      new (&trace_ray) RayDifferential(
          intersection.hit_point.CreateRay(bsdf_sample->direction),
          iris::Ray(intersection.differentials->dx,
                    bsdf_sample->differentials->dx),
          iris::Ray(intersection.differentials->dy,
                    bsdf_sample->differentials->dy));
    } else {
      new (&trace_ray) RayDifferential(
          intersection.hit_point.CreateRay(bsdf_sample->direction));
    }
  }

  return bsdf_sample;
}

}  // namespace internal
}  // namespace integrators
}  // namespace iris