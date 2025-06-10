#include "iris/visibility_tester.h"

#include <limits>

#include "iris/float.h"
#include "iris/internal/ray_tracer.h"
#include "iris/ray.h"

namespace iris {

bool VisibilityTester::Visible(const Ray& ray, geometric_t maximum_distance) {
  bool hit_something =
      ray_tracer_.TraceAnyHit(ray, minimum_distance_, maximum_distance, scene_);
  return !hit_something;
}

bool VisibilityTester::Visible(const Ray& ray) {
  return Visible(ray, std::numeric_limits<geometric_t>::infinity());
}

}  // namespace iris
