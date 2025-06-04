#include "iris/bxdf_allocator.h"

#include <initializer_list>
#include <span>

#include "iris/internal/arena.h"

namespace iris {

void* BxdfAllocator::Allocate(size_t size) { return arena_.Allocate(size); }

std::span<const Bxdf*> BxdfAllocator::AllocateList(
    std::initializer_list<const Bxdf*> bxdfs) {
  const Bxdf** start = static_cast<const Bxdf**>(
      arena_.Allocate(sizeof(const Bxdf*) * bxdfs.size()));

  const Bxdf** end = start;
  for (const Bxdf* bxdf : bxdfs) {
    if (bxdf) {
      *end++ = bxdf;
    }
  }

  return std::span(start, end);
}

}  // namespace iris
