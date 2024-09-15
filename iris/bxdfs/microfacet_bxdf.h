#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdfs/fresnel.h"
#include "iris/bxdfs/microfacet_distribution.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

class MicrofacetBrdf final : public Bxdf {
 public:
  MicrofacetBrdf(const Reflector& reflectance,
                 const MicrofacetDistribution& distribution,
                 const Fresnel& fresnel) noexcept
      : reflectance_(reflectance),
        distribution_(distribution),
        fresnel_(fresnel) {}

  bool IsDiffuse(visual_t* diffuse_pdf) const override;

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override;

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override;

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override;

 private:
  const Reflector& reflectance_;
  const MicrofacetDistribution& distribution_;
  const Fresnel& fresnel_;
};

class MicrofacetBtdf final : public Bxdf {
 public:
  MicrofacetBtdf(const Reflector& transmittance, const geometric_t eta_incident,
                 const geometric_t eta_transmitted,
                 const MicrofacetDistribution& distribution,
                 const Fresnel& fresnel) noexcept
      : transmittance_(transmittance),
        eta_incident_over_transmitted_(eta_incident / eta_transmitted),
        eta_transmitted_over_incident_(eta_transmitted / eta_incident),
        distribution_(distribution),
        fresnel_(fresnel) {}

  bool IsDiffuse(visual_t* diffuse_pdf) const override;

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override;

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override;

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override;

 private:
  geometric_t RelativeRefractiveRatio(const Vector& incident) const;
  geometric_t ReversedRelativeRefractiveRatio(const Vector& incident) const;

  const Reflector& transmittance_;
  const geometric_t eta_incident_over_transmitted_;
  const geometric_t eta_transmitted_over_incident_;
  const MicrofacetDistribution& distribution_;
  const Fresnel& fresnel_;
};

}  // namespace internal

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
class MicrofacetBrdf final : public Bxdf {
 public:
  MicrofacetBrdf(const Reflector& reflectance, const M& distribution,
                 const F& fresnel) noexcept
      : distribution_(distribution),
        fresnel_(fresnel),
        impl_(reflectance, distribution_, fresnel_) {}

  bool IsDiffuse(visual_t* diffuse_pdf) const override {
    return impl_.IsDiffuse(diffuse_pdf);
  }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override {
    return impl_.SampleDiffuse(incoming, surface_normal, sampler);
  }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    return impl_.Sample(incoming, differentials, surface_normal, sampler);
  }

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override {
    return impl_.Pdf(incoming, outgoing, surface_normal, hemisphere);
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    return impl_.Reflectance(incoming, outgoing, hemisphere, allocator);
  }

 private:
  const M distribution_;
  const F fresnel_;
  const internal::MicrofacetBrdf impl_;
};

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
class MicrofacetBtdf final : public Bxdf {
 public:
  MicrofacetBtdf(const Reflector& transmittance, const geometric_t eta_incident,
                 const geometric_t eta_transmitted, const M& distribution,
                 const F& fresnel) noexcept
      : distribution_(distribution),
        fresnel_(fresnel),
        impl_(transmittance, eta_incident, eta_transmitted, distribution_,
              fresnel_) {}

  bool IsDiffuse(visual_t* diffuse_pdf) const override {
    return impl_.IsDiffuse(diffuse_pdf);
  }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override {
    return impl_.SampleDiffuse(incoming, surface_normal, sampler);
  }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    return impl_.Sample(incoming, differentials, surface_normal, sampler);
  }

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override {
    return impl_.Pdf(incoming, outgoing, surface_normal, hemisphere);
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    return impl_.Reflectance(incoming, outgoing, hemisphere, allocator);
  }

 private:
  const M distribution_;
  const F fresnel_;
  const internal::MicrofacetBtdf impl_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_