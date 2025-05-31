#ifndef _IRIS_BXDFS_TRANSPARENT_BTDF_
#define _IRIS_BXDFS_TRANSPARENT_BTDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeTransparentBtdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* transmittance);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_TRANSPARENT_BTDF_
