#include "iris/bxdfs/specular_bxdf.h"

#include <cmath>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {
namespace internal {
namespace {

Bxdf::Hemisphere SampledHemisphere(const Vector& incoming,
                                   const Vector& outgoing) {
  return std::signbit(incoming.z) == std::signbit(outgoing.z)
             ? Bxdf::Hemisphere::BRDF
             : Bxdf::Hemisphere::BTDF;
}

}  // namespace

bool SpecularBrdf::IsDiffuse(visual_t* diffuse_pdf) const {
  if (diffuse_pdf != nullptr) {
    *diffuse_pdf = static_cast<visual_t>(0.0);
  }

  return false;
}

std::optional<Vector> SpecularBrdf::SampleDiffuse(const Vector& incoming,
                                                  const Vector& surface_normal,
                                                  Sampler& sampler) const {
  return std::nullopt;
}

std::optional<Bxdf::SampleResult> SpecularBrdf::Sample(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  Vector reflected(-incoming.x, -incoming.y, incoming.z);
  if (!differentials) {
    return Bxdf::SampleResult{reflected, std::nullopt, this};
  }

  Differentials outgoing_diffs = {
      Vector(-differentials->dx.x, -differentials->dx.y, differentials->dx.z),
      Vector(-differentials->dy.x, -differentials->dy.y, differentials->dy.z)};
  return Bxdf::SampleResult{reflected, outgoing_diffs, this};
}

visual_t SpecularBrdf::Pdf(const Vector& incoming, const Vector& outgoing,
                           const Vector& surface_normal,
                           Hemisphere hemisphere) const {
  return (hemisphere == Hemisphere::BRDF) ? static_cast<visual_t>(1.0)
                                          : static_cast<visual_t>(0.0);
}

const Reflector* SpecularBrdf::Reflectance(const Vector& incoming,
                                           const Vector& outgoing,
                                           Hemisphere hemisphere,
                                           SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BRDF) {
    return nullptr;
  }

  return fresnel_.AttenuateReflectance(reflectance_,
                                       internal::CosTheta(outgoing), allocator);
}

bool SpecularBtdf::IsDiffuse(visual_t* diffuse_pdf) const {
  if (diffuse_pdf != nullptr) {
    *diffuse_pdf = static_cast<visual_t>(0.0);
  }

  return false;
}

std::optional<Vector> SpecularBtdf::SampleDiffuse(const Vector& incoming,
                                                  const Vector& surface_normal,
                                                  Sampler& sampler) const {
  return std::nullopt;
}

std::optional<Bxdf::SampleResult> SpecularBtdf::Sample(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  Vector shading_normal(static_cast<geometric>(0.0),
                        static_cast<geometric>(0.0),
                        static_cast<geometric>(1.0));
  geometric_t relative_refractive_index = RelativeRefractiveIndex(incoming);

  std::optional<Vector> outgoing = internal::Refract(
      incoming, shading_normal.AlignWith(incoming), relative_refractive_index);
  if (!outgoing) {
    return std::nullopt;
  }

  if (!differentials) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<Vector> outgoing_dx = internal::Refract(
      differentials->dx, shading_normal.AlignWith(differentials->dx),
      relative_refractive_index);
  if (!outgoing_dx) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<Vector> outgoing_dy = internal::Refract(
      differentials->dy, shading_normal.AlignWith(differentials->dy),
      relative_refractive_index);
  if (!outgoing_dy) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  return Bxdf::SampleResult{*outgoing,
                            Differentials{*outgoing_dx, *outgoing_dy}, this};
}

visual_t SpecularBtdf::Pdf(const Vector& incoming, const Vector& outgoing,
                           const Vector& surface_normal,
                           Hemisphere hemisphere) const {
  return (hemisphere == Hemisphere::BTDF) ? static_cast<visual_t>(1.0)
                                          : static_cast<visual_t>(0.0);
}

const Reflector* SpecularBtdf::Reflectance(const Vector& incoming,
                                           const Vector& outgoing,
                                           Hemisphere hemisphere,
                                           SpectralAllocator& allocator) const {
  if (hemisphere != Hemisphere::BTDF) {
    return nullptr;
  }

  const Reflector* transmittance = fresnel_.AttenuateTransmittance(
      transmittance_, internal::CosTheta(outgoing), allocator);

  // It may be better to do this in the integrator
  geometric_t relative_refractive_index = RelativeRefractiveIndex(incoming);
  geometric_t attenuation =
      relative_refractive_index * relative_refractive_index;

  return allocator.UnboundedScale(transmittance, attenuation);
}

geometric_t SpecularBtdf::RelativeRefractiveIndex(
    const Vector& incoming) const {
  return relative_refractive_index_[std::signbit(incoming.z)];
}

}  // namespace internal

bool SpecularBxdf::IsDiffuse(visual_t* diffuse_pdf) const {
  if (diffuse_pdf != nullptr) {
    *diffuse_pdf = static_cast<visual_t>(0.0);
  }

  return false;
}

std::optional<Vector> SpecularBxdf::SampleDiffuse(const Vector& incoming,
                                                  const Vector& surface_normal,
                                                  Sampler& sampler) const {
  return std::nullopt;
}

std::optional<Bxdf::SampleResult> SpecularBxdf::Sample(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  geometric_t eta_incident = EtaIncident(incoming);
  geometric_t eta_transmitted = EtaTransmitted(incoming);
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      internal::CosTheta(incoming), eta_incident, eta_transmitted);

  if (sampler.Next() < fresnel_reflectance) {
    Vector outgoing(-incoming.x, -incoming.y, incoming.z);
    if (!differentials) {
      return Bxdf::SampleResult{outgoing, std::nullopt, this};
    }

    Vector outgoing_dx(-differentials->dx.x, -differentials->dx.y,
                       differentials->dx.z);
    Vector outgoing_dy(-differentials->dy.x, -differentials->dy.y,
                       differentials->dy.z);

    return Bxdf::SampleResult{outgoing, Differentials{outgoing_dx, outgoing_dy},
                              this};
  }

  Vector shading_normal(static_cast<geometric>(0.0),
                        static_cast<geometric>(0.0),
                        static_cast<geometric>(1.0));
  geometric_t relative_refractive_index = RelativeRefractiveIndex(incoming);

  std::optional<Vector> outgoing = internal::Refract(
      incoming, shading_normal.AlignWith(incoming), relative_refractive_index);
  if (!outgoing) {
    return std::nullopt;
  }

  if (!differentials) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<Vector> outgoing_dx = internal::Refract(
      differentials->dx, shading_normal.AlignWith(differentials->dx),
      relative_refractive_index);
  if (!outgoing_dx) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<Vector> outgoing_dy = internal::Refract(
      differentials->dy, shading_normal.AlignWith(differentials->dy),
      relative_refractive_index);
  if (!outgoing_dy) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  return Bxdf::SampleResult{*outgoing,
                            Differentials{*outgoing_dx, *outgoing_dy}, this};
}

visual_t SpecularBxdf::Pdf(const Vector& incoming, const Vector& outgoing,
                           const Vector& surface_normal,
                           Hemisphere hemisphere) const {
  if (hemisphere != internal::SampledHemisphere(incoming, outgoing)) {
    return static_cast<visual_t>(0.0);
  }

  geometric_t eta_incident = EtaIncident(incoming);
  geometric_t eta_transmitted = EtaTransmitted(incoming);
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      internal::CosTheta(incoming), eta_incident, eta_transmitted);

  if (hemisphere == Hemisphere::BRDF) {
    return fresnel_reflectance;
  }

  return static_cast<visual_t>(1.0) - fresnel_reflectance;
}

const Reflector* SpecularBxdf::Reflectance(const Vector& incoming,
                                           const Vector& outgoing,
                                           Hemisphere hemisphere,
                                           SpectralAllocator& allocator) const {
  if (hemisphere != internal::SampledHemisphere(incoming, outgoing)) {
    return nullptr;
  }

  geometric_t eta_incident = EtaIncident(incoming);
  geometric_t eta_transmitted = EtaTransmitted(incoming);
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      internal::CosTheta(incoming), eta_incident, eta_transmitted);

  if (hemisphere == Hemisphere::BRDF) {
    return allocator.Scale(&reflectance_, fresnel_reflectance);
  }

  visual_t fresnel_transmittance =
      static_cast<visual_t>(1.0) - fresnel_reflectance;

  // It may be better to do this in the integrator
  geometric_t relative_refractive_index = RelativeRefractiveIndex(incoming);
  geometric_t attenuation =
      relative_refractive_index * relative_refractive_index;

  return allocator.UnboundedScale(&transmittance_,
                                  attenuation * fresnel_transmittance);
}

geometric_t SpecularBxdf::EtaIncident(const Vector& incoming) const {
  return refractive_indices_[std::signbit(incoming.z)];
}

geometric_t SpecularBxdf::EtaTransmitted(const Vector& incoming) const {
  return refractive_indices_[!std::signbit(incoming.z)];
}

geometric_t SpecularBxdf::RelativeRefractiveIndex(
    const Vector& incoming) const {
  return relative_refractive_index_[std::signbit(incoming.z)];
}

}  // namespace bxdfs
}  // namespace iris