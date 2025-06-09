#include "iris/visibility_tester.h"

#include "iris/internal/ray_tracer.h"

namespace iris {

bool VisibilityTester::Visible(const Ray& ray, geometric_t maximum_distance) {
  bool hit_something =
      ray_tracer_.TraceAnyHit(ray, minimum_distance_, maximum_distance, scene_);
  return !hit_something;
}

}  // namespace iris
