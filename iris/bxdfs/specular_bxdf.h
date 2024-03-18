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
    if (std::signbit(incoming.z)) {
      return std::nullopt;
    }

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

    // TODO: Rework Fresnel interface so that MirrorBrdf can be dropped for this
    const Reflector* fresnel =
        fresnel_.Evaluate(internal::CosTheta(outgoing), allocator);
    const Reflector* reflectance =
        allocator.Scale(&reflectance_, static_cast<visual_t>(1.0) /
                                           internal::AbsCosTheta(outgoing));

    return allocator.Scale(reflectance, fresnel);
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
        eta_incident_over_transmitted_(eta_incident / eta_transmitted),
        fresnel_(fresnel) {}

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    if (std::signbit(incoming.z)) {
      return std::nullopt;
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

    const Reflector* fresnel =
        fresnel_.Complement(internal::CosTheta(outgoing), allocator);
    const Reflector* transmittance = allocator.Scale(&transmittance_, fresnel);

    geometric_t attenuation =
        eta_incident_over_transmitted_ * eta_incident_over_transmitted_;
    return allocator.UnboundedScale(transmittance, attenuation);
  }

 private:
  const Reflector& transmittance_;
  const geometric_t eta_incident_over_transmitted_;
  const F fresnel_;
};

class SpecularBxdf final : public Bxdf {
 public:
  SpecularBxdf(const Reflector& reflectance, const Reflector& transmittance,
               const geometric_t eta_incident,
               const geometric_t eta_transmitted) noexcept
      : reflectance_(reflectance),
        transmittance_(transmittance),
        eta_incident_(eta_incident),
        eta_transmitted_(eta_transmitted),
        eta_incident_over_transmitted_(eta_incident / eta_transmitted) {}

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
  const Reflector& reflectance_;
  const Reflector& transmittance_;
  const geometric_t eta_incident_;
  const geometric_t eta_transmitted_;
  const geometric_t eta_incident_over_transmitted_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_SPECULAR_BXDF_