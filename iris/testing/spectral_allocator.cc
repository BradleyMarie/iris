#include "iris/spectral_allocator.h"

#include "iris/internal/arena.h"
#include "iris/spectral_allocator.h"

namespace iris {
namespace testing {

SpectralAllocator& GetSpectralAllocator() {
  thread_local internal::Arena arena;
  thread_local SpectralAllocator spectral_allocator(arena);
  return spectral_allocator;
}

}  // namespace testing
}  // namespace iris
