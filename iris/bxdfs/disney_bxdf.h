#ifndef _IRIS_BXDFS_DISNEY_BXDF_
#define _IRIS_BXDFS_DISNEY_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeDisneyDiffuseBrdf(BxdfAllocator& bxdf_allocator,
                                  const Reflector* color);

const Bxdf* MakeDisneySheenBrdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* color);

const Bxdf* MakeDisneySubsurfaceBrdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* color,
                                     visual_t roughness);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_DISNEY_BXDF_
