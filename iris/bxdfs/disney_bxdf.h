#ifndef _IRIS_BXDFS_DISNEY_BXDF_
#define _IRIS_BXDFS_DISNEY_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeDisneyClearcoatBrdf(BxdfAllocator& bxdf_allocator,
                                    visual_t weight, visual_t gloss);

const Bxdf* MakeDisneyDiffuseBrdf(BxdfAllocator& bxdf_allocator,
                                  const Reflector* color);

const Bxdf* MakeDisneyDiffuseRetroBrdf(BxdfAllocator& bxdf_allocator,
                                       const Reflector* color,
                                       visual_t roughness);

const Bxdf* MakeDisneySheenBrdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* color);

const Bxdf* MakeDisneySubsurfaceBrdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* color,
                                     visual_t roughness);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_DISNEY_BXDF_
