#ifndef _IRIS_BXDFS_OREN_NAYAR_BXDF_
#define _IRIS_BXDFS_OREN_NAYAR_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeOrenNayarBrdf(BxdfAllocator& bxdf_allocator,
                              const Reflector* reflector, visual_t sigma);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_OREN_NAYAR_BXDF_
