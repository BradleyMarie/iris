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

std::optional<Bxdf::SampleResult> SpecularBxdf::Sample(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  if (std::signbit(incoming.z)) {
    return std::nullopt;
  }

  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      internal::CosTheta(incoming), eta_incident_, eta_transmitted_);

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

  std::optional<Vector> outgoing =
      internal::Refract(incoming, shading_normal.AlignWith(incoming),
                        eta_incident_over_transmitted_);
  if (!outgoing) {
    return std::nullopt;
  }

  if (!differentials) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<Vector> outgoing_dx = internal::Refract(
      differentials->dx, shading_normal.AlignWith(differentials->dx),
      eta_incident_over_transmitted_);
  if (!outgoing_dx) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  std::optional<Vector> outgoing_dy = internal::Refract(
      differentials->dy, shading_normal.AlignWith(differentials->dy),
      eta_incident_over_transmitted_);
  if (!outgoing_dy) {
    return Bxdf::SampleResult{*outgoing, std::nullopt, this};
  }

  return Bxdf::SampleResult{*outgoing,
                            Differentials{*outgoing_dx, *outgoing_dy}, this};
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
    return allocator.Scale(&reflectance_, internal::AbsCosTheta(incoming));
  }

  return allocator.Scale(&transmittance_, eta_incident_over_transmitted_ *
                                              eta_incident_over_transmitted_ *
                                              internal::AbsCosTheta(incoming));
}

}  // namespace bxdfs
}  // namespace iris