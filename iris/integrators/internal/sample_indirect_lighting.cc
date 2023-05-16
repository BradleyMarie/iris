#include "iris/integrators/internal/sample_indirect_lighting.h"

namespace iris {
namespace integrators {
namespace internal {

std::optional<Bsdf::SampleResult> SampleIndirectLighting(
    const RayTracer::SurfaceIntersection& intersection, Sampler sampler,
    SpectralAllocator& allocator, RayDifferential& trace_ray) {
  auto bsdf_sample = intersection.bsdf.Sample(trace_ray.direction,
                                              std::move(sampler), allocator);

  if (bsdf_sample) {
    new (&trace_ray) RayDifferential(
        iris::Ray(intersection.hit_point, bsdf_sample->direction));
  }

  return bsdf_sample;
}

}  // namespace internal
}  // namespace integrators
}  // namespace iris