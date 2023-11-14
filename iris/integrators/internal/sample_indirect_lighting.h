#ifndef _IRIS_INTEGRATORS_INTERNAL_SAMPLE_INDIRECT_LIGHTING_
#define _IRIS_INTEGRATORS_INTERNAL_SAMPLE_INDIRECT_LIGHTING_

#include <optional>

#include "iris/ray_differential.h"
#include "iris/ray_tracer.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace integrators {
namespace internal {

std::optional<Bsdf::SampleResult> SampleIndirectLighting(
    const RayTracer::SurfaceIntersection& intersection, Sampler sampler,
    SpectralAllocator& allocator, RayDifferential& trace_ray);

}  // namespace internal
}  // namespace integrators
}  // namespace iris

#endif  // _IRIS_INTEGRATORS_INTERNAL_SAMPLE_INDIRECT_LIGHTING_