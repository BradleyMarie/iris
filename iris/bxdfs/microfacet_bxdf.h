#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

#include <cmath>
#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdfs/fresnel.h"
#include "iris/bxdfs/math.h"
#include "iris/bxdfs/microfacet_distribution.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

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

  bool IsDiffuse(visual_t* diffuse_pdf) const override {
    if (diffuse_pdf != nullptr) {
      *diffuse_pdf = static_cast<visual_t>(1.0);
    }

    return true;
  }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());

    std::optional<Vector> outgoing = internal::Reflect(incoming, half_angle);
    if (!outgoing || outgoing->z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing->z)) {
      return std::nullopt;
    }

    return *outgoing;
  }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    if (incoming.z == static_cast<geometric_t>(0.0)) {
      return std::nullopt;
    }

    Vector half_angle =
        distribution_.Sample(incoming, sampler.Next(), sampler.Next());

    std::optional<Vector> outgoing = internal::Reflect(incoming, half_angle);
    if (!outgoing || outgoing->z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing->z)) {
      return std::nullopt;
    }

    return SampleResult{*outgoing};
  }

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override {
    if (incoming.z == static_cast<geometric>(0.0) ||
        outgoing.z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing.z)) {
      return static_cast<visual_t>(0.0);
    }

    std::optional<Vector> half_angle = internal::HalfAngle(incoming, outgoing);
    if (!half_angle) {
      return static_cast<visual_t>(0.0);
    }

    visual_t dot_product = DotProduct(incoming, *half_angle);
    return distribution_.Pdf(incoming, *half_angle) /
           (static_cast<visual_t>(4.0) * dot_product);
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BRDF ||
        incoming.z == static_cast<geometric>(0.0) ||
        outgoing.z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) != std::signbit(outgoing.z)) {
      return nullptr;
    }

    std::optional<Vector> half_angle = internal::HalfAngle(incoming, outgoing);
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

  bool IsDiffuse(visual_t* diffuse_pdf) const override {
    if (diffuse_pdf != nullptr) {
      *diffuse_pdf = static_cast<visual_t>(1.0);
    }

    return true;
  }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override {
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

    return *outgoing;
  }

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

    return SampleResult{*outgoing};
  }

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override {
    if (incoming.z == static_cast<geometric>(0.0) ||
        outgoing.z == static_cast<geometric>(0.0) ||
        std::signbit(incoming.z) == std::signbit(outgoing.z)) {
      return static_cast<visual_t>(0.0);
    }

    visual_t refractive_ratio = ReversedRelativeRefractiveRatio(incoming);

    std::optional<Vector> half_angle =
        HalfAngle(incoming, outgoing, refractive_ratio);
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
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (hemisphere != Hemisphere::BTDF ||
        (incoming.z == static_cast<geometric>(0.0)) ||
        (outgoing.z == static_cast<geometric>(0.0)) ||
        std::signbit(incoming.z) == std::signbit(outgoing.z)) {
      return nullptr;
    }

    visual_t refractive_ratio = ReversedRelativeRefractiveRatio(incoming);

    std::optional<Vector> maybe_half_angle =
        HalfAngle(incoming, outgoing, refractive_ratio);
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