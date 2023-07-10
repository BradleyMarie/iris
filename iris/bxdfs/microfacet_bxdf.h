#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

#include <algorithm>
#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdfs/fresnel.h"
#include "iris/bxdfs/math.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
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

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
 public:
  TrowbridgeReitzDistribution(visual_t alpha_x, visual_t alpha_y)
      : alpha_x_(std::max(static_cast<visual>(0.001), alpha_x)),
        alpha_y_(std::max(static_cast<visual>(0.001), alpha_y)) {}

  virtual visual_t D(const Vector& vector) const override;
  virtual visual_t Lambda(const Vector& vector) const override;
  virtual Vector Sample(const Vector& incoming, geometric_t u,
                        geometric_t v) const override;

  static visual_t RoughnessToAlpha(visual_t roughness);

 private:
  visual_t alpha_x_, alpha_y_;
};

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
class MicrofacetBrdf final : public Bxdf {
 public:
  MicrofacetBrdf(const Reflector& reflectance, const M& distribution,
                 const F& fresnel) noexcept
      : reflectance_(reflectance),
        distribution_(distribution),
        fresnel_(fresnel) {}

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());
    if (DotProduct(incoming, half_angle) <= static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector outgoing = internal::Reflect(incoming, half_angle);
    if ((incoming.z < static_cast<geometric_t>(0.0)) !=
        (outgoing.z < static_cast<geometric_t>(0.0))) {
      return std::nullopt;
    }

    return SampleResult{outgoing, std::nullopt, this};
  }

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override {
    if (hemisphere != Hemisphere::BRDF ||
        (incoming.z == static_cast<geometric_t>(0.0)) ||
        (outgoing.z == static_cast<geometric_t>(0.0))) {
      return static_cast<geometric_t>(0.0);
    }

    Vector half_angle = Normalize(outgoing + incoming);
    return distribution_.Pdf(incoming, half_angle) /
           (static_cast<visual_t>(4.0) *
            static_cast<visual_t>(DotProduct(incoming, half_angle)));
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BRDF ||
        (incoming.z == static_cast<geometric_t>(0.0)) ||
        (outgoing.z == static_cast<geometric_t>(0.0))) {
      return nullptr;
    }

    Vector half_angle = Normalize(outgoing + incoming);
    visual_t dp = ClampedDotProduct(incoming, half_angle);
    const Reflector* fresnel = fresnel_.Evaluate(dp, allocator);
    if (!fresnel) {
      return nullptr;
    }

    const Reflector* reflectance = allocator.Scale(&reflectance_, fresnel);

    visual_t cos_theta_i = internal::AbsCosTheta(incoming);
    visual_t cos_theta_o = internal::AbsCosTheta(outgoing);
    visual_t d = distribution_.D(half_angle);
    visual_t g = distribution_.G(incoming, outgoing);
    visual_t attenuation =
        d * g / (static_cast<visual_t>(4.0) * cos_theta_i * cos_theta_o);

    return allocator.UnboundedScale(reflectance, attenuation);
  }

 private:
  const Reflector& reflectance_;
  const M distribution_;
  const F fresnel_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_