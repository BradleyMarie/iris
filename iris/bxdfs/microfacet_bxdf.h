#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

#include <algorithm>
#include <cmath>
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
      const Vector& surface_normal, Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());

    Vector outgoing = internal::Reflect(incoming, half_angle);
    if (std::signbit(incoming.z) != std::signbit(outgoing.z)) {
      return std::nullopt;
    }

    return SampleResult{outgoing, std::nullopt, this};
  }

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override {
    if (hemisphere != Hemisphere::BRDF ||
        incoming.z == static_cast<geometric_t>(0.0) ||
        outgoing.z == static_cast<geometric_t>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing.z)) {
      return static_cast<visual_t>(0.0);
    }

    Vector half_angle = Normalize(outgoing + incoming);
    visual_t dot_product = DotProduct(incoming, half_angle);

    return distribution_.Pdf(incoming, half_angle) /
           (static_cast<visual_t>(4.0) * dot_product);
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BRDF ||
        (incoming.z == static_cast<geometric_t>(0.0)) ||
        (outgoing.z == static_cast<geometric_t>(0.0)) ||
        std::signbit(incoming.z) != std::signbit(outgoing.z)) {
      return nullptr;
    }

    Vector half_angle = Normalize(outgoing + incoming);
    visual_t dp = ClampedDotProduct(incoming, half_angle);
    const Reflector* fresnel = fresnel_.Evaluate(dp, allocator);
    if (!fresnel) {
      return nullptr;
    }

    const Reflector* reflectance = allocator.Scale(&reflectance_, fresnel);

    visual_t cos_theta_incident = internal::AbsCosTheta(incoming);
    visual_t cos_theta_outgoing = internal::AbsCosTheta(outgoing);
    visual_t d = distribution_.D(half_angle);
    visual_t g = distribution_.G(incoming, outgoing);
    visual_t attenuation =
        d * g /
        (static_cast<visual_t>(4.0) * cos_theta_incident * cos_theta_outgoing);

    return allocator.UnboundedScale(reflectance, attenuation);
  }

 private:
  const Reflector& reflectance_;
  const M distribution_;
  const F fresnel_;
};

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
class MicrofacetBtdf final : public Bxdf {
 public:
  MicrofacetBtdf(const Reflector& reflectance, const geometric_t eta_incident,
                 const geometric_t eta_transmitted, const M& distribution,
                 const F& fresnel) noexcept
      : reflectance_(reflectance),
        eta_incident_over_transmitted_(eta_incident / eta_transmitted),
        eta_transmitted_over_incident_(eta_transmitted / eta_incident),
        distribution_(distribution),
        fresnel_(fresnel) {}

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());

    std::optional<Vector> outgoing = internal::Refract(
        incoming, half_angle, RelativeRefractiveRatio(incoming));
    if (!outgoing) {
      return std::nullopt;
    }

    if (std::signbit(incoming.z) == std::signbit(outgoing->z)) {
      return std::nullopt;
    }

    return SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override {
    if (hemisphere != Hemisphere::BTDF ||
        incoming.z == static_cast<geometric_t>(0.0) ||
        outgoing.z == static_cast<geometric_t>(0.0) ||
        std::signbit(incoming.z) == std::signbit(outgoing.z)) {
      return static_cast<visual_t>(0.0);
    }

    visual_t refractive_ratio = RelativeRefractiveRatio(incoming);

    auto half_angle = HalfAngle(incoming, outgoing, refractive_ratio);
    if (!half_angle) {
      return static_cast<visual_t>(0.0);
    }

    visual_t dp_incoming = DotProduct(incoming, *half_angle);
    visual_t dp_outgoing = DotProduct(outgoing, *half_angle);

    if (std::signbit(dp_incoming) == std::signbit(dp_outgoing)) {
      return static_cast<visual_t>(0.0);
    }

    visual_t scaled_dp_outgoing = refractive_ratio * dp_outgoing;
    visual_t sqrt_denorm = dp_incoming + scaled_dp_outgoing;
    visual_t dhalf_angle_doutgoing =
        std::abs((refractive_ratio * refractive_ratio * scaled_dp_outgoing) /
                 (sqrt_denorm * sqrt_denorm));

    return distribution_.Pdf(incoming, *half_angle) / dhalf_angle_doutgoing;
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BTDF ||
        (incoming.z == static_cast<geometric_t>(0.0)) ||
        (outgoing.z == static_cast<geometric_t>(0.0)) ||
        std::signbit(incoming.z) == std::signbit(outgoing.z)) {
      return nullptr;
    }

    visual_t refractive_ratio = RelativeRefractiveRatio(incoming);

    auto half_angle = HalfAngle(incoming, outgoing, refractive_ratio);
    if (!half_angle) {
      return nullptr;
    }

    visual_t dp_incoming = DotProduct(incoming, *half_angle);
    visual_t dp_outgoing = DotProduct(outgoing, *half_angle);

    if (std::signbit(dp_incoming) == std::signbit(dp_outgoing)) {
      return nullptr;
    }

    const Reflector* fresnel = fresnel_.Complement(dp_incoming, allocator);
    if (!fresnel) {
      return nullptr;
    }

    visual_t scaled_dp_outgoing = refractive_ratio * dp_outgoing;
    visual_t sqrt_denorm = dp_incoming + scaled_dp_outgoing;
    visual_t scale =
        static_cast<visual_t>(1.0) / (refractive_ratio * refractive_ratio);

    const Reflector* reflectance = allocator.Scale(&reflectance_, fresnel);

    visual_t cos_theta_incident = internal::CosTheta(incoming);
    visual_t cos_theta_outgoing = internal::CosTheta(outgoing);
    visual_t d = distribution_.D(*half_angle);
    visual_t g = distribution_.G(incoming, outgoing);

    visual_t numer = d * g * scale * dp_incoming * dp_outgoing *
                     refractive_ratio * refractive_ratio;
    visual_t denom =
        cos_theta_incident * cos_theta_outgoing * sqrt_denorm * sqrt_denorm;
    visual_t attenuation = std::abs(numer / denom);

    return allocator.UnboundedScale(reflectance, attenuation);
  }

 private:
  geometric_t RelativeRefractiveRatio(const Vector& incident) const {
    return incident.z > static_cast<geometric_t>(0.0)
               ? eta_incident_over_transmitted_
               : eta_transmitted_over_incident_;
  }

  static std::optional<Vector> HalfAngle(const Vector& incoming,
                                         const Vector& outgoing,
                                         geometric_t refractive_ratio) {
    geometric_t x = incoming.x + outgoing.x * refractive_ratio;
    geometric_t y = incoming.y + outgoing.y * refractive_ratio;
    geometric_t z = incoming.z + outgoing.z * refractive_ratio;

    if (x == static_cast<geometric_t>(0.0) &&
        y == static_cast<geometric_t>(0.0) &&
        z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    return Normalize(Vector(x, y, z));
  }

  const Reflector& reflectance_;
  const geometric_t eta_incident_over_transmitted_;
  const geometric_t eta_transmitted_over_incident_;
  const M distribution_;
  const F fresnel_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_