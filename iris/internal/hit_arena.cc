#include "iris/internal/hit_arena.h"

#include <cstring>

#include "iris/float.h"
#include "iris/integer.h"
#include "iris/internal/hit.h"

namespace iris {
namespace internal {

Hit& HitArena::Allocate(iris::Hit* next, geometric distance,
                        geometric_t distance_error, face_t front, face_t back,
                        const void* additional_data,
                        size_t additional_data_size) {
  void* allocated_additional_data = arena_.Allocate(additional_data_size);
  memcpy(allocated_additional_data, additional_data, additional_data_size);

  return arena_.Allocate<Hit>(next, distance, distance_error, front, back,
                              allocated_additional_data);
}

void HitArena::Clear() { arena_.Clear(); }

}  // namespace internal
}  // namespace iris
