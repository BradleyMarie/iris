#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

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

template <typename F>
  requires std::derived_from<F, Fresnel>
class SpecularBrdf final : public Bxdf {
 public:
  SpecularBrdf(const Reflector& reflectance, const F& fresnel) noexcept
      : reflectance_(reflectance), fresnel_(fresnel) {}

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    Vector reflected(-incoming.x, -incoming.y, incoming.z);
    if (!differentials) {
      return Bxdf::SampleResult{reflected, std::nullopt, this};
    }

    Differentials outgoing_diffs = {
        Vector(-differentials->dx.x, -differentials->dx.y, differentials->dx.z),
        Vector(-differentials->dy.x, -differentials->dy.y,
               differentials->dy.z)};
    return Bxdf::SampleResult{reflected, outgoing_diffs, this};
  }

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override {
    if (sample_source != this || hemisphere != Hemisphere::BRDF) {
      return static_cast<visual_t>(0.0);
    }

    return std::nullopt;
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (sample_source != this || hemisphere != Hemisphere::BRDF) {
      return nullptr;
    }

    return fresnel_.AttenuateReflectance(
        reflectance_, internal::CosTheta(outgoing), allocator);
  }

 private:
  const Reflector& reflectance_;
  const F fresnel_;
};

template <typename F>
  requires std::derived_from<F, Fresnel>
class SpecularBtdf final : public Bxdf {
 public:
  SpecularBtdf(const Reflector& transmittance, const geometric_t eta_incident,
               const geometric_t eta_transmitted, const F& fresnel) noexcept
      : transmittance_(transmittance),
        relative_refractive_index_{eta_incident / eta_transmitted,
                                   eta_transmitted / eta_incident},
        fresnel_(fresnel) {}

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    Vector shading_normal(static_cast<geometric>(0.0),
                          static_cast<geometric>(0.0),
                          static_cast<geometric>(1.0));
    geometric_t relative_refractive_index = RelativeRefractiveIndex(incoming);

    std::optional<Vector> outgoing =
        internal::Refract(incoming, shading_normal.AlignWith(incoming),
                          relative_refractive_index);
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

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override {
    if (sample_source != this || hemisphere != Hemisphere::BTDF) {
      return static_cast<visual_t>(0.0);
    }

    return std::nullopt;
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    if (sample_source != this || hemisphere != Hemisphere::BTDF) {
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

 private:
  geometric_t RelativeRefractiveIndex(const Vector& incoming) const {
    return relative_refractive_index_[std::signbit(incoming.z)];
  }

  const Reflector& transmittance_;
  const geometric_t relative_refractive_index_[2];
  const F fresnel_;
};

class SpecularBxdf final : public Bxdf {
 public:
  SpecularBxdf(const Reflector& reflectance, const Reflector& transmittance,
               const geometric_t eta_incident,
               const geometric_t eta_transmitted) noexcept
      : reflectance_(reflectance),
        transmittance_(transmittance),
        refractive_indices_{eta_incident, eta_transmitted},
        relative_refractive_index_{eta_incident / eta_transmitted,
                                   eta_transmitted / eta_incident} {}

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override;

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override;

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override;

 private:
  geometric_t EtaIncident(const Vector& incoming) const {
    return refractive_indices_[std::signbit(incoming.z)];
  }

  geometric_t EtaTransmitted(const Vector& incoming) const {
    return refractive_indices_[!std::signbit(incoming.z)];
  }

  geometric_t RelativeRefractiveIndex(const Vector& incoming) const {
    return relative_refractive_index_[std::signbit(incoming.z)];
  }

  const Reflector& reflectance_;
  const Reflector& transmittance_;
  const geometric_t refractive_indices_[2];
  const geometric_t relative_refractive_index_[2];
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_SPECULAR_BXDF_