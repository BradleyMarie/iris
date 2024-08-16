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

class TrowbridgeReitzDistribution final : public MicrofacetDistribution {
 public:
  TrowbridgeReitzDistribution(visual_t alpha_x, visual_t alpha_y)
      : alpha_x_(std::max(static_cast<visual>(0.001), alpha_x)),
        alpha_y_(std::max(static_cast<visual>(0.001), alpha_y)) {}

  visual_t D(const Vector& vector) const override;
  visual_t Lambda(const Vector& vector) const override;
  Vector Sample(const Vector& incoming, geometric_t u,
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

  bool IsDiffuse() const override { return true; }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());

    auto outgoing = internal::Reflect(incoming, half_angle);
    if (!outgoing || outgoing->z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing->z)) {
      return std::nullopt;
    }

    return SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override {
    if (incoming.z == static_cast<geometric>(0.0) ||
        outgoing.z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing.z)) {
      return static_cast<visual_t>(0.0);
    }

    auto half_angle = internal::HalfAngle(incoming, outgoing);
    if (!half_angle) {
      return static_cast<visual_t>(0.0);
    }

    visual_t dot_product = DotProduct(incoming, *half_angle);
    return distribution_.Pdf(incoming, *half_angle) /
           (static_cast<visual_t>(4.0) * dot_product);
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BRDF ||
        incoming.z == static_cast<geometric>(0.0) ||
        outgoing.z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing.z)) {
      return nullptr;
    }

    auto half_angle = internal::HalfAngle(incoming, outgoing);
    if (!half_angle) {
      return nullptr;
    }

    visual_t dp = ClampedDotProduct(incoming, *half_angle);
    const Reflector* reflectance = fresnel_.AttenuateReflectance(
        reflectance_, std::copysign(dp, incoming.z), allocator);
    if (!reflectance) {
      return nullptr;
    }

    visual_t cos_theta_incident = internal::AbsCosTheta(incoming);
    visual_t cos_theta_outgoing = internal::AbsCosTheta(outgoing);
    visual_t d = distribution_.D(*half_angle);
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
  MicrofacetBtdf(const Reflector& transmittance, const geometric_t eta_incident,
                 const geometric_t eta_transmitted, const M& distribution,
                 const F& fresnel) noexcept
      : transmittance_(transmittance),
        eta_incident_over_transmitted_(eta_incident / eta_transmitted),
        eta_transmitted_over_incident_(eta_transmitted / eta_incident),
        distribution_(distribution),
        fresnel_(fresnel) {}

  bool IsDiffuse() const override { return true; }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());
    if (DotProduct(incoming, half_angle) < static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

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
    if (incoming.z == static_cast<geometric>(0.0) ||
        outgoing.z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) == std::signbit(outgoing.z)) {
      return static_cast<visual_t>(0.0);
    }

    visual_t refractive_ratio = ReversedRelativeRefractiveRatio(incoming);

    auto half_angle = HalfAngle(incoming, outgoing, refractive_ratio);
    if (!half_angle) {
      return static_cast<visual_t>(0.0);
    }

    visual_t dp_incoming = DotProduct(incoming, *half_angle);
    visual_t dp_outgoing = DotProduct(outgoing, *half_angle);

    if (std::signbit(dp_incoming) == std::signbit(dp_outgoing)) {
      return static_cast<visual_t>(0.0);
    }

    visual_t numer = refractive_ratio * refractive_ratio * dp_outgoing;
    visual_t sqrt_denom = dp_incoming + refractive_ratio * dp_outgoing;
    visual_t denom = sqrt_denom * sqrt_denom;

    visual_t dhalf_angle_doutgoing = std::abs(numer / denom);

    return distribution_.Pdf(incoming, *half_angle) * dhalf_angle_doutgoing;
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BTDF ||
        (incoming.z == static_cast<geometric>(0.0)) ||
        (outgoing.z == static_cast<geometric>(0.0)) ||
        std::signbit(incoming.z) == std::signbit(outgoing.z)) {
      return nullptr;
    }

    visual_t refractive_ratio = ReversedRelativeRefractiveRatio(incoming);

    auto maybe_half_angle = HalfAngle(incoming, outgoing, refractive_ratio);
    if (!maybe_half_angle) {
      return nullptr;
    }

    // Unclear why this is required
    Vector half_angle = maybe_half_angle->z < static_cast<geometric>(0.0)
                            ? -*maybe_half_angle
                            : *maybe_half_angle;

    visual_t dp_incoming = DotProduct(incoming, half_angle);
    visual_t dp_outgoing = DotProduct(outgoing, half_angle);

    if (std::signbit(dp_incoming) == std::signbit(dp_outgoing)) {
      return nullptr;
    }

    const Reflector* transmittance =
        fresnel_.AttenuateTransmittance(transmittance_, dp_incoming, allocator);
    if (!transmittance) {
      return nullptr;
    }

    visual_t cos_theta_incident = internal::CosTheta(incoming);
    visual_t cos_theta_outgoing = internal::CosTheta(outgoing);
    visual_t d = distribution_.D(half_angle);
    visual_t g = distribution_.G(incoming, outgoing);
    visual_t sqrt_denom = dp_incoming + refractive_ratio * dp_outgoing;

    visual_t numer =
        d * g * refractive_ratio * refractive_ratio * dp_incoming * dp_outgoing;
    visual_t denom =
        cos_theta_incident * cos_theta_outgoing * sqrt_denom * sqrt_denom;
    visual_t attenuation = std::abs(numer / denom);

    // It may be better to do this in the integrator
    attenuation *=
        static_cast<visual_t>(1.0) / (refractive_ratio * refractive_ratio);

    return allocator.UnboundedScale(transmittance, attenuation);
  }

 private:
  geometric_t RelativeRefractiveRatio(const Vector& incident) const {
    return incident.z > static_cast<geometric>(0.0)
               ? eta_incident_over_transmitted_
               : eta_transmitted_over_incident_;
  }

  geometric_t ReversedRelativeRefractiveRatio(const Vector& incident) const {
    return incident.z > static_cast<geometric>(0.0)
               ? eta_transmitted_over_incident_
               : eta_incident_over_transmitted_;
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

  const Reflector& transmittance_;
  const geometric_t eta_incident_over_transmitted_;
  const geometric_t eta_transmitted_over_incident_;
  const M distribution_;
  const F fresnel_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_