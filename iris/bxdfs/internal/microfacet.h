#ifndef _IRIS_BXDFS_INTERNAL_MICROFACET_
#define _IRIS_BXDFS_INTERNAL_MICROFACET_

#include "iris/float.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

class MicrofacetDistribution {
 public:
  virtual visual_t G(const Vector& incoming, const Vector& outgoing) const;
  visual_t G1(const Vector& vector) const;
  visual_t Pdf(const Vector& incoming, const Vector& half_angle) const;

  virtual visual_t D(const Vector& vector) const = 0;
  virtual visual_t Lambda(const Vector& vector) const = 0;
  virtual Vector Sample(const Vector& incoming, geometric_t u,
                        geometric_t v) const = 0;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
 public:
  TrowbridgeReitzDistribution(geometric_t roughness_or_alpha_x,
                              geometric_t roughness_or_alpha_y,
                              bool is_roughness = true);

  TrowbridgeReitzDistribution(geometric_t roughness_or_alpha,
                              bool is_roughness = true);

  visual_t D(const Vector& vector) const override final;
  visual_t Lambda(const Vector& vector) const override final;
  Vector Sample(const Vector& incoming, geometric_t u,
                geometric_t v) const override final;

 private:
  geometric_t alpha_x_, alpha_y_;
};

class DisneyDistribution : public TrowbridgeReitzDistribution {
 public:
  DisneyDistribution(geometric_t alpha_x, geometric_t alpha_y)
      : TrowbridgeReitzDistribution(alpha_x, alpha_y, false) {}

  visual_t G(const Vector& incoming, const Vector& outgoing) const override;
};

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_INTERNAL_MICROFACET_
