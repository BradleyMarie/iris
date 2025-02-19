#include "iris/internal/ray_tracer.h"

namespace iris {
namespace internal {

Hit* RayTracer::TraceClosestHit(const Ray& ray, geometric_t minimum_distance,
                                geometric_t maximum_distance,
                                const Scene& scene) {
  closest_hit_arena_.Clear();

  iris::Hit* closest_hit;
  Intersector intersector(ray, minimum_distance, maximum_distance,
                          closest_hit_arena_, closest_hit,
                          /*find_any_hit=*/false);
  scene.Trace(ray, intersector);

  return static_cast<Hit*>(closest_hit);
}

Hit* RayTracer::TraceAnyHit(const Ray& ray, geometric_t minimum_distance,
                            geometric_t maximum_distance, const Scene& scene) {
  any_hit_arena_.Clear();

  iris::Hit* any_hit;
  Intersector intersector(ray, minimum_distance, maximum_distance,
                          any_hit_arena_, any_hit, /*find_any_hit=*/true);
  scene.Trace(ray, intersector);

  return static_cast<Hit*>(any_hit);
}

}  // namespace internal
}  // namespace iris