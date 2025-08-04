#ifndef _IRIS_BXDFS_ATTENUATED_BXDF_
#define _IRIS_BXDFS_ATTENUATED_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/reflector.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeAttenuatedBxdf(BxdfAllocator& allocator, const Bxdf* bxdf,
                               const Reflector* attenuation);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_LAMBERTIAN_BXDF_
