#ifndef _IRIS_BXDFS_MICROFACET_DISTRIBUTIONS_TROWBRIDGE_REITZ_DISTRIBUTION_
#define _IRIS_BXDFS_MICROFACET_DISTRIBUTIONS_TROWBRIDGE_REITZ_DISTRIBUTION_

#include <algorithm>

#include "iris/bxdfs/microfacet_distribution.h"
#include "iris/float.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace microfacet_distributions {

class TrowbridgeReitzDistribution final : public MicrofacetDistribution {
 public:
  TrowbridgeReitzDistribution(visual_t alpha_x, visual_t alpha_y)
      : alpha_x_(std::max(static_cast<visual_t>(0.001), alpha_x)),
        alpha_y_(std::max(static_cast<visual_t>(0.001), alpha_y)) {}

  visual_t D(const Vector& vector) const override;
  visual_t Lambda(const Vector& vector) const override;
  Vector Sample(const Vector& incoming, geometric_t u,
                geometric_t v) const override;

  static visual_t RoughnessToAlpha(visual_t roughness);

 private:
  visual_t alpha_x_, alpha_y_;
};

}  // namespace microfacet_distributions
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_DISTRIBUTIONS_TROWBRIDGE_REITZ_DISTRIBUTION_