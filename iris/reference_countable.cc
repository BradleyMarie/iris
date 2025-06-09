#include "iris/reference_countable.h"

#undef NDEBUG  // Enable assertions at runtime
#include <cassert>
#include <limits>

namespace iris {

void ReferenceCountable::Increment() noexcept {
  auto old_value = reference_count_.fetch_add(1u, std::memory_order_relaxed);
  assert(old_value != std::numeric_limits<reference_count_t>::max());
}

bool ReferenceCountable::Decrement() noexcept {
  auto old_value = reference_count_.fetch_sub(1u, std::memory_order_relaxed);
  assert(old_value != 0);
  return old_value == 1;
}

}  // namespace iris
