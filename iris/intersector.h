#ifndef _IRIS_INTERSECTOR_
#define _IRIS_INTERSECTOR_

#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/hit.h"
#include "iris/matrix.h"
#include "iris/ray.h"

namespace iris {
namespace internal {
class HitArena;
}  // namespace internal

class Intersector final {
 public:
  Intersector(const Ray& ray, geometric_t minimum_distance,
              geometric_t maximum_distance, internal::HitArena& hit_arena,
              Hit*& hit, bool find_any_hit) noexcept
      : ray_(ray),
        minimum_distance_(minimum_distance),
        closest_hit_distance_(maximum_distance),
        maximum_distance_(maximum_distance),
        hit_arena_(hit_arena),
        hit_(hit),
        find_any_hit_(find_any_hit),
        done_(false) {
    hit_ = nullptr;
  }

  // If `true` is returned, no further intersections are required.
  bool Intersect(const Geometry& geometry);

  // If `true` is returned, no further intersections are required.
  bool Intersect(const Geometry& geometry, const Matrix* model_to_world);

  // If `true` is returned, no further intersections are required.
  bool Intersect(const Geometry& geometry, const Matrix& model_to_world);

  geometric_t MinimumDistance() const { return minimum_distance_; }
  geometric_t MaximumDistance() const { return closest_hit_distance_; }

 private:
  Intersector(const Intersector&) = delete;
  Intersector& operator=(const Intersector&) = delete;

  const Ray& ray_;
  geometric_t minimum_distance_;
  geometric_t closest_hit_distance_;
  geometric_t maximum_distance_;
  internal::HitArena& hit_arena_;
  Hit*& hit_;
  bool find_any_hit_;
  bool done_;
};

}  // namespace iris

#endif  // _IRIS_INTERSECTOR_
