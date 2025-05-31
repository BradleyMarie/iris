#include "iris/bxdfs/microfacet_bxdf.h"

#include <cmath>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

std::optional<Vector> HalfAngle(const Vector& incoming, const Vector& outgoing,
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

}  // namespace

std::optional<Vector> MicrofacetBrdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
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

visual_t MicrofacetBrdf::PdfDiffuse(const Vector& incoming,
                                    const Vector& outgoing,
                                    const Vector& surface_normal,
                                    Hemisphere hemisphere) const {
  if (incoming.z == static_cast<geometric>(0.0) ||
      outgoing.z == static_cast<geometric>(0.0) ||
      std::signbit(incoming.z) != std::signbit(outgoing.z)) {
    return static_cast<visual_t>(0.0);
  }

  std::optional<Vector> half_angle = internal::HalfAngle(incoming, outgoing);
  if (!half_angle) {
    return static_cast<visual_t>(0.0);
  }

  visual_t dot_product = ClampedDotProduct(incoming, *half_angle);
  return distribution_.Pdf(incoming, *half_angle) /
         (static_cast<visual_t>(4.0) * dot_product);
}

const Reflector* MicrofacetBrdf::ReflectanceDiffuse(
    const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
    SpectralAllocator& allocator) const {
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

std::optional<Vector> MicrofacetBtdf::SampleDiffuse(
    const Vector& incoming, const Vector& surface_normal,
    Sampler& sampler) const {
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

visual_t MicrofacetBtdf::PdfDiffuse(const Vector& incoming,
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
      HalfAngle(incoming, outgoing, refractive_ratio);
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

const Reflector* MicrofacetBtdf::ReflectanceDiffuse(
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
      HalfAngle(incoming, outgoing, refractive_ratio);
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
  attenuation /= refractive_ratio * refractive_ratio;

  return allocator.UnboundedScale(transmittance, attenuation);
}

geometric_t MicrofacetBtdf::RelativeRefractiveRatio(
    const Vector& incident) const {
  return incident.z > static_cast<geometric>(0.0)
             ? eta_incident_over_transmitted_
             : eta_transmitted_over_incident_;
}

geometric_t MicrofacetBtdf::ReversedRelativeRefractiveRatio(
    const Vector& incident) const {
  return incident.z > static_cast<geometric>(0.0)
             ? eta_transmitted_over_incident_
             : eta_incident_over_transmitted_;
}

}  // namespace internal

const Reflector* FresnelDielectric::AttenuateReflectance(
    const Reflector& reflectance, visual_t cos_theta_incident,
    SpectralAllocator& allocator) const {
  geometric_t eta_incident =
      std::signbit(cos_theta_incident) ? eta_back_ : eta_front_;
  geometric_t eta_transmitted =
      std::signbit(cos_theta_incident) ? eta_front_ : eta_back_;
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
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
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      cos_theta_incident, eta_incident, eta_transmitted);
  return allocator.Scale(&transmittance,
                         static_cast<visual_t>(1.0) - fresnel_reflectance);
}

bool FresnelDielectric::IsValid() const {
  return std::isfinite(eta_front_) &&
         eta_front_ >= static_cast<visual_t>(1.0) && std::isfinite(eta_back_) &&
         eta_back_ >= static_cast<visual_t>(1.0);
}

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
         eta_dielectric_ > static_cast<visual_t>(0.0) && eta_conductor_;
}

}  // namespace bxdfs
}  // namespace iris
