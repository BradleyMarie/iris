#include "iris/testing/hit_allocator.h"

#include <memory>
#include <vector>

#include "iris/internal/hit_arena.h"

namespace iris {
namespace testing {

HitAllocator MakeHitAllocator(const Ray& ray) {
  thread_local std::vector<std::unique_ptr<Ray>> rays;
  thread_local internal::HitArena arena;

  rays.push_back(std::make_unique<Ray>(ray));

  return HitAllocator(*rays.back(), arena);
}

face_t FrontFace(const Hit& hit) {
  auto internal_hit = static_cast<const internal::Hit&>(hit);
  return internal_hit.front;
}

face_t BackFace(const Hit& hit) {
  auto internal_hit = static_cast<const internal::Hit&>(hit);
  return internal_hit.back;
}

const void* AdditionalData(const Hit& hit) {
  auto internal_hit = static_cast<const internal::Hit&>(hit);
  return internal_hit.additional_data;
}

}  // namespace testing
}  // namespace iris