#ifndef _IRIS_BXDFS_MIRROR_BTDF_
#define _IRIS_BXDFS_MIRROR_BTDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeMirrorBrdf(BxdfAllocator& bxdf_allocator,
                           const Reflector* reflectance);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MIRROR_BTDF_
