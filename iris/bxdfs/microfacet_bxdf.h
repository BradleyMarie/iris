#ifndef _IRIS_BXDFS_MICROFACET_BXDF_
#define _IRIS_BXDFS_MICROFACET_BXDF_

#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/fresnel.h"
#include "iris/bxdfs/helpers/diffuse_bxdf.h"
#include "iris/bxdfs/microfacet_distribution.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

class MicrofacetBrdf final : public helpers::DiffuseBxdf {
 public:
  MicrofacetBrdf(const Reflector& reflectance,
                 const MicrofacetDistribution& distribution,
                 const Fresnel& fresnel) noexcept
      : reflectance_(reflectance),
        distribution_(distribution),
        fresnel_(fresnel) {}

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override;

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& reflectance_;
  const MicrofacetDistribution& distribution_;
  const Fresnel& fresnel_;
};

class MicrofacetBtdf final : public helpers::DiffuseBxdf {
 public:
  MicrofacetBtdf(const Reflector& transmittance, geometric_t eta_incident,
                 geometric_t eta_transmitted,
                 const MicrofacetDistribution& distribution,
                 const Fresnel& fresnel) noexcept
      : transmittance_(transmittance),
        eta_incident_over_transmitted_(eta_incident / eta_transmitted),
        eta_transmitted_over_incident_(eta_transmitted / eta_incident),
        distribution_(distribution),
        fresnel_(fresnel) {}

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override;

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
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
const Bxdf* MakeMicrofacetBrdf(BxdfAllocator& bxdf_allocator,
                               const Reflector* reflectance,
                               const M& distribution, const F& fresnel) {
  class MicrofacetBrdf final : public helpers::DiffuseBxdf {
   public:
    MicrofacetBrdf(const Reflector& reflectance, const M& distribution,
                   const F& fresnel) noexcept
        : distribution_(distribution),
          fresnel_(fresnel),
          impl_(reflectance, distribution_, fresnel_) {}

    std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                        const Vector& surface_normal,
                                        Sampler& sampler) const override {
      return impl_.SampleDiffuse(incoming, surface_normal, sampler);
    }

    visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                        const Vector& surface_normal,
                        Hemisphere hemisphere) const override {
      return impl_.PdfDiffuse(incoming, outgoing, surface_normal, hemisphere);
    }

    const Reflector* ReflectanceDiffuse(
        const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
        SpectralAllocator& allocator) const override {
      return impl_.ReflectanceDiffuse(incoming, outgoing, hemisphere,
                                      allocator);
    }

   private:
    const M distribution_;
    const F fresnel_;
    const internal::MicrofacetBrdf impl_;
  };

  if (!reflectance) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<MicrofacetBrdf>(*reflectance, distribution,
                                                  fresnel);
}

template <typename M, typename F>
  requires std::derived_from<M, MicrofacetDistribution> &&
           std::derived_from<F, Fresnel>
const Bxdf* MakeMicrofacetBtdf(BxdfAllocator& bxdf_allocator,
                               const Reflector* transmittance,
                               geometric_t eta_incident,
                               geometric_t eta_transmitted,
                               const M& distribution, const F& fresnel) {
  class MicrofacetBtdf final : public helpers::DiffuseBxdf {
   public:
    MicrofacetBtdf(const Reflector& transmittance, geometric_t eta_incident,
                   geometric_t eta_transmitted, const M& distribution,
                   const F& fresnel) noexcept
        : distribution_(distribution),
          fresnel_(fresnel),
          impl_(transmittance, eta_incident, eta_transmitted, distribution_,
                fresnel_) {}

    std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                        const Vector& surface_normal,
                                        Sampler& sampler) const override {
      return impl_.SampleDiffuse(incoming, surface_normal, sampler);
    }

    visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                        const Vector& surface_normal,
                        Hemisphere hemisphere) const override {
      return impl_.PdfDiffuse(incoming, outgoing, surface_normal, hemisphere);
    }

    const Reflector* ReflectanceDiffuse(
        const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
        SpectralAllocator& allocator) const override {
      return impl_.ReflectanceDiffuse(incoming, outgoing, hemisphere,
                                      allocator);
    }

   private:
    const M distribution_;
    const F fresnel_;
    const internal::MicrofacetBtdf impl_;
  };

  if (!transmittance) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<MicrofacetBtdf>(
      *transmittance, eta_incident, eta_transmitted, distribution, fresnel);
}

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_MICROFACET_BXDF_
