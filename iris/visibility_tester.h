#ifndef _IRIS_VISIBILITY_TESTER_
#define _IRIS_VISIBILITY_TESTER_

#include "iris/float.h"
#include "iris/ray.h"
#include "iris/scene.h"

namespace iris {
namespace internal {
class Arena;
class RayTracer;
class VisibilityTester;
}  // namespace internal

class VisibilityTester {
 public:
  VisibilityTester(const Scene& scene, geometric_t minimum_distance,
                   internal::RayTracer& ray_tracer,
                   internal::Arena& arena) noexcept
      : scene_(scene),
        minimum_distance_(minimum_distance),
        ray_tracer_(ray_tracer),
        arena_(arena) {}

  bool Visible(const Ray& ray, geometric_t maximum_distance);
  bool Visible(const Ray& ray);

 private:
  VisibilityTester(const VisibilityTester&) = delete;
  VisibilityTester& operator=(const VisibilityTester&) = delete;

  const Scene& scene_;
  geometric_t minimum_distance_;
  internal::RayTracer& ray_tracer_;
  internal::Arena& arena_;

  friend class internal::VisibilityTester;
};

}  // namespace iris

#endif  // _IRIS_VISIBILITY_TESTER_
