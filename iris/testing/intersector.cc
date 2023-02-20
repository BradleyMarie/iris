#include "iris/testing/intersector.h"

#include <memory>
#include <vector>

#include "iris/internal/hit_arena.h"

namespace iris {
namespace testing {

Intersector MakeIntersector(const Ray& ray, geometric_t minimum_distance,
                            geometric_t maximum_distance, Hit*& closest_hit) {
  thread_local std::vector<std::unique_ptr<Ray>> rays;
  thread_local internal::HitArena arena;

  rays.push_back(std::make_unique<Ray>(ray));

  return Intersector(*rays.back(), minimum_distance, maximum_distance, arena,
                     closest_hit);
}

}  // namespace testing
}  // namespace iris