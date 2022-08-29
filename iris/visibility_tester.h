#ifndef _IRIS_VISIBILITY_TESTER_
#define _IRIS_VISIBILITY_TESTER_

#include <limits>

#include "iris/float.h"
#include "iris/ray.h"
#include "iris/scene.h"

namespace iris {
namespace internal {
class RayTracer;
class VisibilityTester;
}  // namespace internal

class VisibilityTester {
 public:
  VisibilityTester(const Scene& scene, geometric_t minimum_distance,
                   internal::RayTracer& ray_tracer)
      : scene_(scene),
        minimum_distance_(minimum_distance),
        ray_tracer_(ray_tracer) {}

  bool Visible(const Ray& ray,
               geometric_t maximum_distance =
                   std::numeric_limits<geometric_t>::infinity());

 private:
  VisibilityTester(const VisibilityTester&) = delete;
  VisibilityTester& operator=(const VisibilityTester&) = delete;

  const Scene& scene_;
  geometric_t minimum_distance_;
  internal::RayTracer& ray_tracer_;

  friend class internal::VisibilityTester;
};

}  // namespace iris

#endif  // _IRIS_VISIBILITY_TESTER_