#include "iris/bxdfs/specular_bxdf.h"

#include <cmath>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {
namespace {

constexpr Vector kShadingNormal(static_cast<geometric>(0.0),
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
    const Reflector* transmittance, visual_t relative_refractive_index,
    visual_t pdf, SpectralAllocator& allocator) {
  if (!transmittance) {
    return std::nullopt;
  }

  std::optional<Vector> outgoing = internal::Refract(
      incoming, kShadingNormal.AlignWith(incoming), relative_refractive_index);
  if (!outgoing) {
    return std::nullopt;
  }

  // It may be better to scale by relative_refractive_index in the integrator
  transmittance = allocator.UnboundedScale(
      transmittance, relative_refractive_index * relative_refractive_index);

  return Bxdf::SpecularSample{
      Bxdf::Hemisphere::BTDF, *outgoing, transmittance,
      GetRefractedDifferentials(incoming, differentials,
                                relative_refractive_index),
      pdf};
}

}  // namespace

namespace internal {

std::optional<Bxdf::SpecularSample> SpecularBrdf::SampleSpecular(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler,
    SpectralAllocator& allocator) const {
  const Reflector* reflectance = fresnel_.AttenuateReflectance(
      reflectance_, internal::CosTheta(incoming), allocator);

  std::optional<Bxdf::SpecularSample> result = SampleSpecularReflection(
      incoming, differentials, reflectance, static_cast<visual_t>(1.0));

  return result;
}

std::optional<Bxdf::SpecularSample> SpecularBtdf::SampleSpecular(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler,
    SpectralAllocator& allocator) const {
  const Reflector* transmittance = fresnel_.AttenuateTransmittance(
      transmittance_, internal::CosTheta(incoming), allocator);

  std::optional<Bxdf::SpecularSample> result = SampleSpecularTransmission(
      incoming, differentials, transmittance,
      relative_refractive_index_[std::signbit(incoming.z)],
      static_cast<visual_t>(1.0), allocator);

  return result;
}

}  // namespace internal

std::optional<Bxdf::SpecularSample> SpecularBxdf::SampleSpecular(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler,
    SpectralAllocator& allocator) const {
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      internal::CosTheta(incoming),
      refractive_indices_[std::signbit(incoming.z)],
      refractive_indices_[!std::signbit(incoming.z)]);

  if (visual_t sample = sampler.Next(); fresnel_reflectance < sample) {
    visual_t fresnel_transmittance =
        static_cast<visual_t>(1.0) - fresnel_reflectance;

    std::optional<Bxdf::SpecularSample> result = SampleSpecularTransmission(
        incoming, differentials,
        allocator.Scale(&transmittance_, fresnel_transmittance),
        relative_refractive_index_[std::signbit(incoming.z)],
        fresnel_transmittance, allocator);

    return result;
  }

  std::optional<Bxdf::SpecularSample> result = SampleSpecularReflection(
      incoming, differentials,
      allocator.Scale(&reflectance_, fresnel_reflectance), fresnel_reflectance);

  return result;
}

}  // namespace bxdfs
}  // namespace iris