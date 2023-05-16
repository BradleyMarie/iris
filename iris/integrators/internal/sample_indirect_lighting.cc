#include "iris/integrators/internal/sample_indirect_lighting.h"

namespace iris {
namespace integrators {
namespace internal {

std::optional<Bsdf::SampleResult> SampleIndirectLighting(
    const RayTracer::SurfaceIntersection& intersection, Sampler sampler,
    SpectralAllocator& allocator, RayDifferential& trace_ray) {
  std::optional<Bsdf::Differentials> differentials =
      trace_ray.differentials
          ? Bsdf::Differentials{trace_ray.differentials->dx.direction,
                                trace_ray.differentials->dy.direction}
          : std::optional<Bsdf::Differentials>();

  auto bsdf_sample = intersection.bsdf.Sample(
      trace_ray.direction, differentials, std::move(sampler), allocator);

  if (bsdf_sample) {
    new (&trace_ray) RayDifferential(
        iris::Ray(intersection.hit_point, bsdf_sample->direction));
  }

  return bsdf_sample;
}

}  // namespace internal
}  // namespace integrators
}  // namespace iris