#ifndef _IRIS_BXDFS_DISNEY_BXDF_
#define _IRIS_BXDFS_DISNEY_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeDisneyClearcoatBrdf(BxdfAllocator& bxdf_allocator,
                                    visual_t clearcoat, visual_t gloss);

const Bxdf* MakeDisneyDiffuseBrdf(BxdfAllocator& bxdf_allocator,
                                  const Reflector* color);

const Bxdf* MakeDisneyDiffuseRetroBrdf(BxdfAllocator& bxdf_allocator,
                                       const Reflector* color,
                                       visual_t roughness);

const Bxdf* MakeDisneySpecularBrdf(BxdfAllocator& bxdf_allocator,
                                   const Reflector* color, visual_t metallic,
                                   visual_t specular_tint,
                                   geometric_t eta_incident,
                                   geometric_t eta_transmitted,
                                   geometric_t anisotropic,
                                   geometric_t roughness);

const Bxdf* MakeDisneySpecularBtdf(BxdfAllocator& bxdf_allocator,
                                   const Reflector* color,
                                   geometric_t eta_incident,
                                   geometric_t eta_transmitted,
                                   geometric_t anisotropic,
                                   geometric_t roughness);

const Bxdf* MakeDisneyThinSpecularBtdf(BxdfAllocator& bxdf_allocator,
                                       const Reflector* color,
                                       geometric_t eta_incident,
                                       geometric_t eta_transmitted,
                                       geometric_t anisotropic,
                                       geometric_t roughness);

const Bxdf* MakeDisneySheenBrdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* color, visual_t sheen,
                                visual_t sheen_tint);

const Bxdf* MakeDisneySubsurfaceBrdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* color,
                                     visual_t roughness);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_DISNEY_BXDF_
