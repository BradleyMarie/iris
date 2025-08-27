#ifndef _IRIS_INTERNAL_RAY_TRACER_
#define _IRIS_INTERNAL_RAY_TRACER_

#include "iris/float.h"
#include "iris/internal/hit.h"
#include "iris/internal/hit_arena.h"
#include "iris/ray.h"
#include "iris/scene.h"

namespace iris {
namespace internal {

class RayTracer final {
 public:
  RayTracer() = default;

  Hit* TraceClosestHit(const Ray& ray, geometric_t minimum_distance,
                       geometric_t maximum_distance, const Scene& scene);

  Hit* TraceAnyHit(const Ray& ray, geometric_t minimum_distance,
                   geometric_t maximum_distance, const Scene& scene);

 private:
  RayTracer(const RayTracer&) = delete;
  RayTracer& operator=(const RayTracer&) = delete;

  HitArena closest_hit_arena_;
  HitArena any_hit_arena_;
};

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_RAY_TRACER_
