#ifndef _IRIS_BXDFS_INTERNAL_MATH_
#define _IRIS_BXDFS_INTERNAL_MATH_

#include "iris/float.h"
#include "iris/sampler.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

visual_t FesnelDielectricReflectance(visual_t cos_theta_incident,
                                     visual_t eta_incident,
                                     visual_t eta_transmission);

Vector CosineSampleHemisphere(geometric incoming_z, Sampler& sampler);

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_INTERNAL_MATH_