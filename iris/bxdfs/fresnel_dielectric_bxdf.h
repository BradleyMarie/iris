#ifndef _IRIS_BXDFS_FRESNEL_DIELECTRIC_BXDF_
#define _IRIS_BXDFS_FRESNEL_DIELECTRIC_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeFresnelDielectricBxdf(BxdfAllocator& bxdf_allocator,
                                      const Reflector* reflectance,
                                      const Reflector* transmittance,
                                      geometric_t eta_incident,
                                      geometric_t eta_transmitted);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_FRESNEL_DIELECTRIC_BXDF_
