#ifndef _IRIS_BXDFS_ASHIKHMIN_SHIRLEY_BRDF_
#define _IRIS_BXDFS_ASHIKHMIN_SHIRLEY_BRDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeAshikhminShirleyBrdf(BxdfAllocator& bxdf_allocator,
                                     const Reflector* diffuse,
                                     const Reflector* specular,
                                     geometric_t roughness_x,
                                     geometric_t roughness_y,
                                     bool remap_roughness_to_alpha);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_ASHIKHMIN_SHIRLEY_BRDF_
