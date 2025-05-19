#ifndef _IRIS_RAY_TRACER_
#define _IRIS_RAY_TRACER_

#include <optional>

#include "iris/bsdf.h"
#include "iris/environmental_light.h"
#include "iris/hit_point.h"
#include "iris/float.h"
#include "iris/ray_differential.h"
#include "iris/scene.h"
#include "iris/spectrum.h"

namespace iris {
namespace internal {
class Arena;
class RayTracer;
}  // namespace internal

class RayTracer final {
 public:
  RayTracer(const Scene& scene, const EnvironmentalLight* environmental_light,
            geometric_t minimum_distance, internal::RayTracer& ray_tracer,
            internal::Arena& arena) noexcept
      : scene_(scene),
        environmental_light_(environmental_light),
        minimum_distance_(minimum_distance),
        ray_tracer_(ray_tracer),
        arena_(arena) {}

  struct Differentials {
    const Point dx;
    const Point dy;
  };

  struct SurfaceIntersection {
    const Bsdf bsdf;
    const HitPoint hit_point;
    const std::optional<Differentials> differentials;
    const Vector surface_normal;
    const Vector shading_normal;
  };

  struct TraceResult {
    const Spectrum* const emission;
    const std::optional<SurfaceIntersection> surface_intersection;
  };

  TraceResult Trace(const RayDifferential& ray);

 private:
  RayTracer(const RayTracer&) = delete;
  RayTracer& operator=(const RayTracer&) = delete;

  const Scene& scene_;
  const EnvironmentalLight* environmental_light_;
  geometric_t minimum_distance_;
  internal::RayTracer& ray_tracer_;
  internal::Arena& arena_;
};

}  // namespace iris

#endif  // _IRIS_RAY_TRACER_
