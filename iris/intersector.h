#ifndef _IRIS_INTERSECTOR_
#define _IRIS_INTERSECTOR_

#include "iris/geometry.h"
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
              Hit*& closest_hit) noexcept
      : ray_(ray),
        minimum_distance_(minimum_distance),
        closest_hit_distance_(maximum_distance),
        maximum_distance_(maximum_distance),
        hit_arena_(hit_arena),
        closest_hit_(closest_hit) {
    closest_hit_ = nullptr;
  }

  void Intersect(const Geometry& geometry);
  void Intersect(const Geometry& geometry, const Matrix* model_to_world);
  void Intersect(const Geometry& geometry, const Matrix& model_to_world);

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
  Hit*& closest_hit_;
};

}  // namespace iris

#endif  // _IRIS_INTERSECTOR_