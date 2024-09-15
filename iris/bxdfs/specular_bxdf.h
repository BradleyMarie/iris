#ifndef _IRIS_BXDFS_SPECULAR_BXDF_
#define _IRIS_BXDFS_SPECULAR_BXDF_

#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdfs/fresnel.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

class SpecularBrdf final : public Bxdf {
 public:
  SpecularBrdf(const Reflector& reflectance, const Fresnel& fresnel) noexcept
      : reflectance_(reflectance), fresnel_(fresnel) {}

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
  const Fresnel& fresnel_;
};

class SpecularBtdf final : public Bxdf {
 public:
  SpecularBtdf(const Reflector& transmittance, const geometric_t eta_incident,
               const geometric_t eta_transmitted,
               const Fresnel& fresnel) noexcept
      : transmittance_(transmittance),
        relative_refractive_index_{eta_incident / eta_transmitted,
                                   eta_transmitted / eta_incident},
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
  geometric_t RelativeRefractiveIndex(const Vector& incoming) const;

  const Reflector& transmittance_;
  const geometric_t relative_refractive_index_[2];
  const Fresnel& fresnel_;
};

}  // namespace internal

template <typename F>
  requires std::derived_from<F, Fresnel>
class SpecularBrdf final : public Bxdf {
 public:
  SpecularBrdf(const Reflector& reflectance, const F& fresnel) noexcept
      : fresnel_(fresnel), impl_(reflectance, fresnel_) {}

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
  const F fresnel_;
  const internal::SpecularBrdf impl_;
};

template <typename F>
  requires std::derived_from<F, Fresnel>
class SpecularBtdf final : public Bxdf {
 public:
  SpecularBtdf(const Reflector& transmittance, const geometric_t eta_incident,
               const geometric_t eta_transmitted, const F& fresnel) noexcept
      : fresnel_(fresnel),
        impl_(transmittance, eta_incident, eta_transmitted, fresnel_) {}

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
  const F fresnel_;
  const internal::SpecularBtdf impl_;
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
  geometric_t EtaIncident(const Vector& incoming) const;
  geometric_t EtaTransmitted(const Vector& incoming) const;
  geometric_t RelativeRefractiveIndex(const Vector& incoming) const;

  const Reflector& reflectance_;
  const Reflector& transmittance_;
  const geometric_t refractive_indices_[2];
  const geometric_t relative_refractive_index_[2];
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_SPECULAR_BXDF_