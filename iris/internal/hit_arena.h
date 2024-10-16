#ifndef _IRIS_INTERNAL_HIT_ARENA_
#define _IRIS_INTERNAL_HIT_ARENA_

#include <memory>
#include <vector>

#include "iris/internal/arena.h"
#include "iris/internal/hit.h"

namespace iris {
namespace internal {

class HitArena final {
 public:
  HitArena() = default;

  Hit& Allocate(iris::Hit* next, geometric distance, geometric_t distance_error,
                face_t front, face_t back, const void* additional_data,
                size_t additional_data_size);
  void Clear();

 private:
  HitArena(const HitArena&) = delete;
  HitArena& operator=(const HitArena&) = delete;

  Arena arena_;
};

}  // namespace internal
}  // namespace iris

#endif  // _IRIS_INTERNAL_HIT_ARENA_