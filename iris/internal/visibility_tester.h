#ifndef _IRIS_INTERNAL_VISIBILITY_TESTER_
#define _IRIS_INTERNAL_VISIBILITY_TESTER_

#include <optional>

#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/integer.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/matrix.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/scene.h"
#include "iris/spectrum.h"
#include "iris/visibility_tester.h"

namespace iris {
namespace internal {

class VisibilityTester final : public iris::VisibilityTester {
 public:
  VisibilityTester(const Scene& scene, geometric_t minimum_distance,
                   internal::RayTracer& ray_tracer,
                   internal::Arena& arena) noexcept
      : iris::VisibilityTester(scene, minimum_distance, ray_tracer, arena) {}

  struct VisibleResult {
    const Spectrum& emission;
    Point hit_point;
    visual_t pdf;
  };

  std::optional<VisibleResult> Visible(const Ray& ray, const Geometry& geometry,
                                       const Matrix* model_to_world,
                                       face_t face);

 private:
  VisibilityTester(const VisibilityTester&) = delete;
  VisibilityTester& operator=(const VisibilityTester&) = delete;
};

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_INTERNAL_VISIBILITY_TESTER_
