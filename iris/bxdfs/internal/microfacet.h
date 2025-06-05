#ifndef _IRIS_BXDFS_INTERNAL_MICROFACET_
#define _IRIS_BXDFS_INTERNAL_MICROFACET_

#include "iris/float.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

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

class TrowbridgeReitzDistribution final : public MicrofacetDistribution {
 public:
  TrowbridgeReitzDistribution(geometric_t roughness_or_alpha_x,
                              geometric_t roughness_or_alpha_y,
                              bool is_roughness = true);

  TrowbridgeReitzDistribution(geometric_t roughness_or_alpha,
                              bool is_roughness = true);

  visual_t D(const Vector& vector) const override;
  visual_t Lambda(const Vector& vector) const override;
  Vector Sample(const Vector& incoming, geometric_t u,
                geometric_t v) const override;

 private:
  geometric_t alpha_x_, alpha_y_;
};

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_INTERNAL_MICROFACET_
