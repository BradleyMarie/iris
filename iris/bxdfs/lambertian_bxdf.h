#ifndef _IRIS_BXDFS_LAMBERTIAN_BXDF_
#define _IRIS_BXDFS_LAMBERTIAN_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeLambertianBrdf(BxdfAllocator& bxdf_allocator,
                               const Reflector* reflector);
const Bxdf* MakeLambertianBtdf(BxdfAllocator& bxdf_allocator,
                               const Reflector* reflector);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_LAMBERTIAN_BXDF_
