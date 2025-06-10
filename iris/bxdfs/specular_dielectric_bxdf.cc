#include "iris/bxdfs/specular_dielectric_bxdf.h"

#include <algorithm>
#include <cmath>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/internal/math.h"
#include "iris/bxdfs/internal/specular_bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

class SpecularDielectricBxdf final : public internal::SpecularBxdf {
 public:
  SpecularDielectricBxdf(const Reflector* reflectance,
                         const Reflector* transmittance,
                         const geometric_t eta_incident,
                         const geometric_t eta_transmitted) noexcept
      : reflectance_(reflectance),
        transmittance_(transmittance),
        eta_incident_(eta_incident),
        eta_transmitted_(eta_transmitted) {}

  std::optional<Bxdf::SpecularSample> SampleSpecular(
      const Vector& incoming,
      const std::optional<Bxdf::Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector* reflectance_;
  const Reflector* transmittance_;
  const geometric_t eta_incident_;
  const geometric_t eta_transmitted_;
};

static const Vector kShadingNormal(static_cast<geometric>(0.0),
                                   static_cast<geometric>(0.0),
                                   static_cast<geometric>(1.0));

Vector GetReflectedVector(const Vector& incoming) {
  return Vector(-incoming.x, -incoming.y, incoming.z);
}

std::optional<Bxdf::Differentials> GetReflectedDifferentials(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials) {
  if (!differentials) {
    return std::nullopt;
  }

  return Bxdf::Differentials{GetReflectedVector(differentials->dx),
                             GetReflectedVector(differentials->dy)};
}

std::optional<Bxdf::Differentials> GetRefractedDifferentials(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    geometric_t relative_refractive_index) {
  if (!differentials) {
    return std::nullopt;
  }

  std::optional<Vector> outgoing_dx = internal::Refract(
      differentials->dx, kShadingNormal.AlignWith(differentials->dx),
      relative_refractive_index);
  if (!outgoing_dx) {
    return std::nullopt;
  }

  std::optional<Vector> outgoing_dy = internal::Refract(
      differentials->dy, kShadingNormal.AlignWith(differentials->dy),
      relative_refractive_index);
  if (!outgoing_dy) {
    return std::nullopt;
  }

  return Bxdf::Differentials{*outgoing_dx, *outgoing_dy};
}

std::optional<Bxdf::SpecularSample> SampleSpecularReflection(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Reflector* reflectance, visual_t pdf) {
  if (!reflectance) {
    return std::nullopt;
  }

  return Bxdf::SpecularSample{
      Bxdf::Hemisphere::BRDF, GetReflectedVector(incoming), reflectance,
      GetReflectedDifferentials(incoming, differentials), pdf};
}

std::optional<Bxdf::SpecularSample> SampleSpecularTransmission(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Reflector* transmittance, geometric_t eta_incident,
    geometric_t eta_transmitted, visual_t pdf, SpectralAllocator& allocator) {
  if (!transmittance) {
    return std::nullopt;
  }

  geometric_t relative_refractive_index = eta_incident / eta_transmitted;

  std::optional<Vector> outgoing = internal::Refract(
      incoming, kShadingNormal.AlignWith(incoming), relative_refractive_index);
  if (!outgoing) {
    return std::nullopt;
  }

  // It may be better to scale by relative_refractive_index in the integrator
  transmittance = allocator.UnboundedScale(
      transmittance, static_cast<visual_t>(relative_refractive_index *
                                           relative_refractive_index));

  return Bxdf::SpecularSample{
      Bxdf::Hemisphere::BTDF, *outgoing, transmittance,
      GetRefractedDifferentials(incoming, differentials,
                                relative_refractive_index),
      pdf};
}

std::optional<Bxdf::SpecularSample> SpecularDielectricBxdf::SampleSpecular(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler,
    SpectralAllocator& allocator) const {
  geometric_t eta_incident = eta_incident_;
  geometric_t eta_transmitted = eta_transmitted_;

  if (!std::isfinite(eta_incident) ||
      eta_incident < static_cast<visual_t>(1.0) ||
      !std::isfinite(eta_transmitted) ||
      eta_transmitted < static_cast<visual_t>(1.0)) {
    return std::nullopt;
  }

  if (std::signbit(incoming.z)) {
    std::swap(eta_incident, eta_transmitted);
  }

  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      internal::CosTheta(incoming), eta_incident, eta_transmitted);
  visual_t fresnel_transmittance =
      static_cast<visual_t>(1.0) - fresnel_reflectance;

  bool sample_brdf = reflectance_;
  visual_t pdf = static_cast<visual_t>(1.0);
  if (reflectance_ && transmittance_) {
    sample_brdf = sampler.Next() < fresnel_reflectance;
    pdf = sample_brdf ? fresnel_reflectance : fresnel_transmittance;
  }

  return sample_brdf
             ? SampleSpecularReflection(
                   incoming, differentials,
                   allocator.Scale(reflectance_, fresnel_reflectance), pdf)
             : SampleSpecularTransmission(
                   incoming, differentials,
                   allocator.Scale(transmittance_, fresnel_transmittance),
                   eta_incident, eta_transmitted, pdf, allocator);
}

}  // namespace

const Bxdf* MakeSpecularDielectricBxdf(BxdfAllocator& bxdf_allocator,
                                       const Reflector* reflectance,
                                       const Reflector* transmittance,
                                       geometric_t eta_incident,
                                       geometric_t eta_transmitted) {
  if (!std::isfinite(eta_incident) ||
      eta_incident < static_cast<visual_t>(1.0) ||
      !std::isfinite(eta_transmitted) ||
      eta_transmitted < static_cast<visual_t>(1.0)) {
    return nullptr;
  }

  if (!reflectance && !transmittance) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<SpecularDielectricBxdf>(
      reflectance, transmittance, eta_incident, eta_transmitted);
}

}  // namespace bxdfs
}  // namespace iris
