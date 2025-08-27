#include "iris/bxdfs/microfacet_bxdf.h"

#include <cmath>
#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/diffuse_bxdf.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/bxdfs/internal/microfacet.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

using ::iris::bxdfs::internal::AbsCosTheta;
using ::iris::bxdfs::internal::CosTheta;
using ::iris::bxdfs::internal::FesnelDielectricReflectance;
using ::iris::bxdfs::internal::HalfAngle;
using ::iris::bxdfs::internal::MicrofacetDistribution;
using ::iris::bxdfs::internal::Reflect;
using ::iris::bxdfs::internal::Refract;
using ::iris::bxdfs::internal::TrowbridgeReitzDistribution;

std::optional<Vector> WeightedHalfAngle(const Vector& incoming,
                                        const Vector& outgoing,
                                        geometric_t refractive_ratio) {
  geometric_t x = incoming.x + outgoing.x * refractive_ratio;
  geometric_t y = incoming.y + outgoing.y * refractive_ratio;
  geometric_t z = incoming.z + outgoing.z * refractive_ratio;

  Vector weighted_half_angle(x, y, z);
  if (weighted_half_angle.IsZero()) {
    return std::nullopt;
  }

  return Normalize(weighted_half_angle);
}

class Fresnel {
 public:
  virtual const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const = 0;
  virtual const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const = 0;
  virtual bool IsValid() const = 0;
};

class FresnelDielectric final : public Fresnel {
 public:
  FresnelDielectric(visual_t eta_front, visual_t eta_back)
      : eta_front_(eta_front), eta_back_(eta_back) {}

  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  bool IsValid() const override;

 private:
  visual_t eta_front_;
  visual_t eta_back_;
};

const Reflector* FresnelDielectric::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  geometric_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  geometric_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = FesnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&reflectance, fresnel_reflectance);
}

const Reflector* FresnelDielectric::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  geometric_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  geometric_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = FesnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&transmittance,
                         static_cast<visual_t>(1.0) - fresnel_reflectance);
}

bool FresnelDielectric::IsValid() const {
  return std::isfinite(eta_front_) &&
         eta_front_ >= static_cast<visual_t>(1.0) && std::isfinite(eta_back_) &&
         eta_back_ >= static_cast<visual_t>(1.0);
}

class FresnelConductor final : public Fresnel {
 public:
  FresnelConductor(visual_t eta_dielectric, const Spectrum* eta_conductor,
                   const Spectrum* k_conductor)
      : eta_dielectric_(eta_dielectric),
        eta_conductor_(eta_conductor),
        k_conductor_(k_conductor) {}

  const Reflector* AttenuateReflectance(
      const Reflector& reflectance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  const Reflector* AttenuateTransmittance(
      const Reflector& transmittance, visual_t cos_theta_incident,
      SpectralAllocator& allocator) const override;
  bool IsValid() const override;

 private:
  visual_t eta_dielectric_;
  const Spectrum* eta_conductor_;
  const Spectrum* k_conductor_;
};

const Reflector* FresnelConductor::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  const Reflector* fresnel = allocator.FresnelConductor(
      eta_dielectric_, eta_conductor_, k_conductor_, cos_theta_incident);
  return allocator.Scale(&reflectance, fresnel);
}

const Reflector* FresnelConductor::AttenuateTransmittance(
    const Reflector& transmittance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  return nullptr;
}

bool FresnelConductor::IsValid() const {
  return std::isfinite(eta_dielectric_) &&
         eta_dielectric_ >= static_cast<visual_t>(1.0);
}

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
class MicrofacetBrdf final : public internal::DiffuseBxdf {
 public:
  MicrofacetBrdf(const Reflector& reflectance, const M& distribution,
                 const F& fresnel) noexcept
      : reflectance_(reflectance),
        distribution_(distribution),
        fresnel_(fresnel) {}

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override;

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& reflectance_;
  M distribution_;
  F fresnel_;
};

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
std::optional<Vector> MicrofacetBrdf<M, F>::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  if (incoming.z == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  geometric_t u = sampler.Next();
  geometric_t v = sampler.Next();
  Vector half_angle = distribution_.Sample(incoming, u, v);

  std::optional<Vector> outgoing = Reflect(incoming, half_angle);
  if (!outgoing || outgoing->z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing->z)) {
    return std::nullopt;
  }

  return *outgoing;
}

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
visual_t MicrofacetBrdf<M, F>::PdfDiffuse(const Vector& incoming,
                                          const Vector& outgoing,
                                          const Vector& surface_normal,
                                          Hemisphere hemisphere) const {
  if (incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing.z)) {
    return static_cast<visual_t>(0.0);
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return static_cast<visual_t>(0.0);
  }

  visual_t dot_product = ClampedDotProduct(incoming, *half_angle);
  return distribution_.Pdf(incoming, *half_angle) /
         (static_cast<visual_t>(4.0) * dot_product);
}

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
const Reflector* MicrofacetBrdf<M, F>::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF ||
      incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing.z)) {
    return nullptr;
  }

  std::optional<Vector> half_angle = HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return nullptr;
  }

  visual_t dp = ClampedDotProduct(incoming, *half_angle);
  const Reflector* reflectance = fresnel_.AttenuateReflectance(
      reflectance_, std::copysign(dp, incoming.z), allocator);
  if (!reflectance) {
    return nullptr;
  }

  visual_t cos_theta_incident = AbsCosTheta(incoming);
  visual_t cos_theta_outgoing = AbsCosTheta(outgoing);
  visual_t d = distribution_.D(*half_angle);
  visual_t g = distribution_.G(incoming, outgoing);
  visual_t attenuation =
      d * g /
      (static_cast<visual_t>(4.0) * cos_theta_incident * cos_theta_outgoing);

  return allocator.UnboundedScale(reflectance, attenuation);
}

template <typename M>
  requires std::derived_from<M, MicrofacetDistribution>
class MicrofacetBtdf final : public internal::DiffuseBxdf {
 public:
  MicrofacetBtdf(const Reflector& transmittance, const M& distribution,
                 geometric_t eta_incident, geometric_t eta_transmitted) noexcept
      : transmittance_(transmittance),
        eta_incident_over_transmitted_(eta_incident / eta_transmitted),
        eta_transmitted_over_incident_(eta_transmitted / eta_incident),
        distribution_(distribution),
        fresnel_(eta_incident, eta_transmitted) {}

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override;

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  geometric_t RelativeRefractiveRatio(const Vector& incident) const;
  geometric_t ReversedRelativeRefractiveRatio(const Vector& incident) const;

  const Reflector& transmittance_;
  geometric_t eta_incident_over_transmitted_;
  geometric_t eta_transmitted_over_incident_;
  M distribution_;
  FresnelDielectric fresnel_;
};

template <typename M>
  requires std::derived_from<M, MicrofacetDistribution>
std::optional<Vector> MicrofacetBtdf<M>::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
  if (incoming.z == static_cast<geometric>(0.0)) {
    return std::nullopt;
  }

  geometric_t u = sampler.Next();
  geometric_t v = sampler.Next();
  Vector half_angle = distribution_.Sample(incoming, u, v);
  if (DotProduct(incoming, half_angle) < static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  std::optional<Vector> outgoing =
      Refract(incoming, half_angle, RelativeRefractiveRatio(incoming));
  if (!outgoing) {
    return std::nullopt;
  }

  if (std::signbit(incoming.z) == std::signbit(outgoing->z)) {
    return std::nullopt;
  }

  return *outgoing;
}

template <typename M>
  requires std::derived_from<M, MicrofacetDistribution>
visual_t MicrofacetBtdf<M>::PdfDiffuse(const Vector& incoming,
                                       const Vector& outgoing,
                                       const Vector& surface_normal,
                                       Hemisphere hemisphere) const {
  if (incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) == std::signbit(outgoing.z)) {
    return static_cast<visual_t>(0.0);
  }

  visual_t refractive_ratio = ReversedRelativeRefractiveRatio(incoming);

  std::optional<Vector> half_angle =
      WeightedHalfAngle(incoming, outgoing, refractive_ratio);
  if (!half_angle) {
    return static_cast<visual_t>(0.0);
  }

  visual_t dp_incoming = ClampedDotProduct(incoming, *half_angle);
  visual_t dp_outgoing = ClampedDotProduct(outgoing, *half_angle);

  if (std::signbit(dp_incoming) == std::signbit(dp_outgoing)) {
    return static_cast<visual_t>(0.0);
  }

  visual_t numer = refractive_ratio * refractive_ratio * dp_outgoing;
  visual_t sqrt_denom = dp_incoming + refractive_ratio * dp_outgoing;
  visual_t denom = sqrt_denom * sqrt_denom;

  visual_t dhalf_angle_doutgoing = std::abs(numer / denom);

  return distribution_.Pdf(incoming, *half_angle) * dhalf_angle_doutgoing;
}

template <typename M>
  requires std::derived_from<M, MicrofacetDistribution>
const Reflector* MicrofacetBtdf<M>::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BTDF ||
      (incoming.z == static_cast<geometric>(0.0)) ||
      (outgoing.z == static_cast<geometric>(0.0)) ||
      std::signbit(incoming.z) == std::signbit(outgoing.z)) {
    return nullptr;
  }

  visual_t refractive_ratio = ReversedRelativeRefractiveRatio(incoming);

  std::optional<Vector> maybe_half_angle =
      WeightedHalfAngle(incoming, outgoing, refractive_ratio);
  if (!maybe_half_angle) {
    return nullptr;
  }

  // Unclear why this is required
  Vector half_angle = maybe_half_angle->z < static_cast<geometric>(0.0)
                          ? -*maybe_half_angle
                          : *maybe_half_angle;

  visual_t dp_incoming = ClampedDotProduct(incoming, half_angle);
  visual_t dp_outgoing = ClampedDotProduct(outgoing, half_angle);

  if (std::signbit(dp_incoming) == std::signbit(dp_outgoing)) {
    return nullptr;
  }

  const Reflector* transmittance =
      fresnel_.AttenuateTransmittance(transmittance_, dp_incoming, allocator);
  if (!transmittance) {
    return nullptr;
  }

  visual_t cos_theta_incident = CosTheta(incoming);
  visual_t cos_theta_outgoing = CosTheta(outgoing);
  visual_t d = distribution_.D(half_angle);
  visual_t g = distribution_.G(incoming, outgoing);
  visual_t sqrt_denom = dp_incoming + refractive_ratio * dp_outgoing;

  visual_t numer =
      d * g * refractive_ratio * refractive_ratio * dp_incoming * dp_outgoing;
  visual_t denom =
      cos_theta_incident * cos_theta_outgoing * sqrt_denom * sqrt_denom;
  visual_t attenuation = std::abs(numer / denom);

  // It may be better to do this in the integrator
  attenuation /= refractive_ratio * refractive_ratio;

  return allocator.UnboundedScale(transmittance, attenuation);
}

template <typename M>
  requires std::derived_from<M, MicrofacetDistribution>
geometric_t MicrofacetBtdf<M>::RelativeRefractiveRatio(
    const Vector& incident) const {
  return incident.z > static_cast<geometric>(0.0)
             ? eta_incident_over_transmitted_
             : eta_transmitted_over_incident_;
}

template <typename M>
  requires std::derived_from<M, MicrofacetDistribution>
geometric_t MicrofacetBtdf<M>::ReversedRelativeRefractiveRatio(
    const Vector& incident) const {
  return incident.z > static_cast<geometric>(0.0)
             ? eta_transmitted_over_incident_
             : eta_incident_over_transmitted_;
}

}  // namespace

const Bxdf* MakeMicrofacetConductorBrdf(
    BxdfAllocator& bxdf_allocator, const Reflector* reflectance,
    visual_t eta_dielectric, const Spectrum* eta_conductor,
    const Spectrum* k_conductor, geometric_t roughness_x,
    geometric_t roughness_y, bool remap_roughness_to_alpha) {
  if (!reflectance) {
    return nullptr;
  }

  FresnelConductor fresnel(eta_dielectric, eta_conductor, k_conductor);
  if (!fresnel.IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<
      MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelConductor>>(
      *reflectance,
      TrowbridgeReitzDistribution(roughness_x, roughness_y,
                                  remap_roughness_to_alpha),
      fresnel);
}

const Bxdf* MakeMicrofacetDielectricBrdf(BxdfAllocator& bxdf_allocator,
                                         const Reflector* reflectance,
                                         geometric_t eta_incident,
                                         geometric_t eta_transmitted,
                                         geometric_t roughness_x,
                                         geometric_t roughness_y,
                                         bool remap_roughness_to_alpha) {
  if (!reflectance) {
    return nullptr;
  }

  FresnelDielectric fresnel(eta_incident, eta_transmitted);
  if (!fresnel.IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<
      MicrofacetBrdf<TrowbridgeReitzDistribution, FresnelDielectric>>(
      *reflectance,
      TrowbridgeReitzDistribution(roughness_x, roughness_y,
                                  remap_roughness_to_alpha),
      fresnel);
}

const Bxdf* MakeMicrofacetDielectricBtdf(BxdfAllocator& bxdf_allocator,
                                         const Reflector* transmittance,
                                         geometric_t eta_incident,
                                         geometric_t eta_transmitted,
                                         geometric_t roughness_x,
                                         geometric_t roughness_y,
                                         bool remap_roughness_to_alpha) {
  if (!transmittance ||
      !FresnelDielectric(eta_incident, eta_transmitted).IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<MicrofacetBtdf<TrowbridgeReitzDistribution>>(
      *transmittance,
      TrowbridgeReitzDistribution(roughness_x, roughness_y,
                                  remap_roughness_to_alpha),
      eta_incident, eta_transmitted);
}

}  // namespace bxdfs
}  // namespace iris
