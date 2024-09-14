#ifndef _IRIS_BXDFS_MICROFACET_DISTRIBUTION_
#define _IRIS_BXDFS_MICROFACET_DISTRIBUTION_

#include "iris/float.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

class MicrofacetDistribution {
 public:
  visual_t G(const Vector& incoming, const Vector& outgoing) const;
  visual_t G1(const Vector& vector) const;
  visual_t Pdf(const Vector& incoming, const Vector& half_angle) const;

  virtual visual_t D(const Vector& vector) const = 0;
  virtual visual_t Lambda(const Vector& vector) const = 0;
  virtual Vector Sample(const Vector& incoming, geometric_t u,
                        geometric_t v) const = 0;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_DISTRIBUTION_