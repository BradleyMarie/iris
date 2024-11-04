#include "iris/bxdfs/specular_bxdf.h"

#include <cmath>

#include "iris/bxdfs/internal/math.h"

namespace iris {
namespace bxdfs {
namespace {

constexpr Vector kShadingNormal(static_cast<geometric>(0.0),
                                static_cast<geometric>(0.0),
                                static_cast<geometric>(1.0));

std::optional<Bxdf::Differentials> GetReflectedDifferentials(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials) {
  if (!differentials) {
    return std::nullopt;
  }

  return Bxdf::Differentials{
      Vector(-differentials->dx.x, -differentials->dx.y, differentials->dx.z),
      Vector(-differentials->dy.x, -differentials->dy.y, differentials->dy.z)};
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

}  // namespace

namespace internal {

std::optional<Bxdf::SpecularSample> SpecularBrdf::SampleSpecular(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  Vector reflected(-incoming.x, -incoming.y, incoming.z);
  std::optional<Differentials> outgoing_diffs =
      GetReflectedDifferentials(incoming, differentials);

  return Bxdf::SpecularSample{Bxdf::Hemisphere::BRDF, reflected, reflectance_,
                              outgoing_diffs, static_cast<visual_t>(1.0)};
}

std::optional<Bxdf::SpecularSample> SpecularBtdf::SampleSpecular(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  geometric_t relative_refractive_index =
      relative_refractive_index_[std::signbit(incoming.z)];

  std::optional<Vector> outgoing = internal::Refract(
      incoming, kShadingNormal.AlignWith(incoming), relative_refractive_index);
  if (!outgoing) {
    return std::nullopt;
  }

  std::optional<Differentials> outgoing_diffs = GetRefractedDifferentials(
      incoming, differentials, relative_refractive_index);

  // It may be better to do this in the integrator
  visual_t pdf = static_cast<visual_t>(1.0) /
                 static_cast<visual_t>(relative_refractive_index *
                                       relative_refractive_index);

  return Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, *outgoing, transmittance_,
                              outgoing_diffs, pdf};
}

}  // namespace internal

std::optional<Bxdf::SpecularSample> SpecularBxdf::SampleSpecular(
    const Vector& incoming,
    const std::optional<Bxdf::Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler) const {
  geometric_t eta_incident = refractive_indices_[std::signbit(incoming.z)];
  geometric_t eta_transmitted = refractive_indices_[!std::signbit(incoming.z)];
  visual_t fresnel_reflectance = internal::FesnelDielectricReflectance(
      internal::CosTheta(incoming), eta_incident, eta_transmitted);

  if (sampler.Next() < fresnel_reflectance) {
    Vector outgoing(-incoming.x, -incoming.y, incoming.z);
    std::optional<Bxdf::Differentials> outgoing_diffs =
        GetReflectedDifferentials(incoming, differentials);

    visual_t pdf = fresnel_reflectance;

    return Bxdf::SpecularSample{Bxdf::Hemisphere::BRDF, outgoing, reflectance_,
                                outgoing_diffs, pdf};
  }

  geometric_t relative_refractive_index =
      relative_refractive_index_[std::signbit(incoming.z)];

  std::optional<Vector> outgoing = internal::Refract(
      incoming, kShadingNormal.AlignWith(incoming), relative_refractive_index);
  if (!outgoing) {
    return std::nullopt;
  }

  std::optional<Bxdf::Differentials> outgoing_diffs = GetRefractedDifferentials(
      incoming, differentials, relative_refractive_index);

  visual_t pdf = static_cast<visual_t>(1.0) - fresnel_reflectance;

  // It may be better to do this in the integrator
  pdf /= static_cast<visual_t>(relative_refractive_index *
                               relative_refractive_index);

  return Bxdf::SpecularSample{Bxdf::Hemisphere::BTDF, *outgoing, transmittance_,
                              outgoing_diffs, pdf};
}

}  // namespace bxdfs
}  // namespace iris