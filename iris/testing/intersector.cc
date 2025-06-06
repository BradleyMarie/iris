#include "iris/testing/intersector.h"

#include <memory>
#include <vector>

#include "iris/float.h"
#include "iris/hit.h"
#include "iris/internal/hit_arena.h"
#include "iris/intersector.h"
#include "iris/ray.h"

namespace iris {
namespace testing {

Intersector MakeIntersector(const Ray& ray, geometric_t minimum_distance,
                            geometric_t maximum_distance, Hit*& closest_hit) {
  thread_local std::vector<std::unique_ptr<Ray>> rays;
  thread_local internal::HitArena arena;

  rays.push_back(std::make_unique<Ray>(ray));

  return Intersector(*rays.back(), minimum_distance, maximum_distance, arena,
                     closest_hit, /*find_any_hit=*/false);
}

}  // namespace testing
}  // namespace iris
