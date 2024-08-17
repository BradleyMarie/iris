#define _USE_MATH_DEFINES
#include "iris/bxdfs/specular_bxdf.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "iris/bxdfs/internal/math.h"
#include "iris/bxdfs/math.h"

namespace iris {
namespace bxdfs {
namespace {

Bxdf::Hemisphere SampledHemisphere(const Vector& incoming,
                                   const Vector& outgoing) {
  return std::signbit(incoming.z) == std::signbit(outgoing.z)
             ? Bxdf::Hemisphere::BRDF
             : Bxdf::Hemisphere::BTDF;
}

}  // namespace

bool SpecularBxdf::IsDiffuse() const { return false; }

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
      return Bxdf::SampleResult{outgoing, std::nullopt, this,
                                static_cast<visual_t>(1.0)};
    }

    Vector outgoing_dx(-differentials->dx.x, -differentials->dx.y,
                       differentials->dx.z);
    Vector outgoing_dy(-differentials->dy.x, -differentials->dy.y,
                       differentials->dy.z);

    return Bxdf::SampleResult{outgoing, Differentials{outgoing_dx, outgoing_dy},
                              this, static_cast<visual_t>(1.0)};
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
    return Bxdf::SampleResult{*outgoing, std::nullopt, this,
                              static_cast<visual_t>(1.0)};
  }

  std::optional<Vector> outgoing_dx = internal::Refract(
      differentials->dx, shading_normal.AlignWith(differentials->dx),
      relative_refractive_index);
  if (!outgoing_dx) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this,
                              static_cast<visual_t>(1.0)};
  }

  std::optional<Vector> outgoing_dy = internal::Refract(
      differentials->dy, shading_normal.AlignWith(differentials->dy),
      relative_refractive_index);
  if (!outgoing_dy) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this,
                              static_cast<visual_t>(1.0)};
  }

  return Bxdf::SampleResult{*outgoing,
                            Differentials{*outgoing_dx, *outgoing_dy}, this,
                            static_cast<visual_t>(1.0)};
}

std::optional<visual_t> SpecularBxdf::Pdf(const Vector& incoming,
                                          const Vector& outgoing,
                                          const Vector& surface_normal,
                                          const Bxdf* sample_source,
                                          Hemisphere hemisphere) const {
  if (sample_source != this ||
      hemisphere != SampledHemisphere(incoming, outgoing)) {
    return static_cast<visual_t>(0.0);
  }

  return std::nullopt;
}

const Reflector* SpecularBxdf::Reflectance(const Vector& incoming,
                                           const Vector& outgoing,
                                           const Bxdf* sample_source,
                                           Hemisphere hemisphere,
                                           SpectralAllocator& allocator) const {
  if (sample_source != this ||
      hemisphere != SampledHemisphere(incoming, outgoing)) {
    return nullptr;
  }

  if (hemisphere == Hemisphere::BRDF) {
    return &reflectance_;
  }

  // It may be better to do this in the integrator
  geometric_t relative_refractive_index = RelativeRefractiveIndex(incoming);
  geometric_t attenuation =
      relative_refractive_index * relative_refractive_index;

  return allocator.UnboundedScale(&transmittance_, attenuation);
}

}  // namespace bxdfs
}  // namespace iris