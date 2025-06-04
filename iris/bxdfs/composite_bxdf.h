#ifndef _IRIS_BXDFS_COMPOSITE_BXDF_
#define _IRIS_BXDFS_COMPOSITE_BXDF_

#include <concepts>
#include <initializer_list>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeCompositeBxdf(BxdfAllocator& allocator,
                              std::initializer_list<const Bxdf*> bxdfs);

const Bxdf* MakeCompositeBxdf(BxdfAllocator& allocator,
                              std::convertible_to<const Bxdf*> auto... args) {
  return MakeCompositeBxdf(allocator, {args...});
}

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_COMPOSITE_BXDF_
