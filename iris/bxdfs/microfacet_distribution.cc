#include "iris/bxdfs/microfacet_distribution.h"

#include "iris/bxdfs/math.h"

namespace iris {
namespace bxdfs {

using ::iris::bxdfs::internal::AbsCosTheta;

visual_t MicrofacetDistribution::G(const Vector& incoming,
                                   const Vector& outgoing) const {
  return static_cast<visual_t>(1.0) /
         (static_cast<visual_t>(1.0) + Lambda(incoming) + Lambda(outgoing));
}

visual_t MicrofacetDistribution::G1(const Vector& vector) const {
  return static_cast<visual_t>(1.0) /
         (static_cast<visual_t>(1.0) + Lambda(vector));
}

visual_t MicrofacetDistribution::Pdf(const Vector& incoming,
                                     const Vector& half_angle) const {
  return D(half_angle) * G1(incoming) * AbsDotProduct(incoming, half_angle) /
         AbsCosTheta(incoming);
}

}  // namespace bxdfs
}  // namespace iris