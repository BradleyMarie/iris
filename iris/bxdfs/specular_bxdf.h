#ifndef _IRIS_BXDFS_SPECULAR_BXDF_
#define _IRIS_BXDFS_SPECULAR_BXDF_

#include <cmath>
#include <concepts>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/fresnel.h"
#include "iris/bxdfs/helpers/specular_bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

class SpecularBrdf final : public helpers::SpecularBxdf {
 public:
  SpecularBrdf(const Reflector& reflectance, const Fresnel& fresnel) noexcept
      : reflectance_(reflectance), fresnel_(fresnel) {}

  std::optional<Bxdf::SpecularSample> SampleSpecular(
      const Vector& incoming,
      const std::optional<Bxdf::Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& reflectance_;
  const Fresnel& fresnel_;
};

}  // namespace internal

template <typename F>
  requires std::derived_from<F, Fresnel>
const Bxdf* MakeSpecularBrdf(BxdfAllocator& bxdf_allocator,
                             const Reflector* reflectance, const F& fresnel) {
  class SpecularBrdf final : public helpers::SpecularBxdf {
   public:
    SpecularBrdf(const Reflector& reflectance, const F& fresnel) noexcept
        : fresnel_(fresnel), impl_(reflectance, fresnel_) {}

    std::optional<Bxdf::SpecularSample> SampleSpecular(
        const Vector& incoming,
        const std::optional<Bxdf::Differentials>& differentials,
        const Vector& surface_normal, Sampler& sampler,
        SpectralAllocator& allocator) const override {
      return impl_.SampleSpecular(incoming, differentials, surface_normal,
                                  sampler, allocator);
    }

   private:
    const F fresnel_;
    const internal::SpecularBrdf impl_;
  };

  if (!reflectance) {
    return nullptr;
  }

  if (!fresnel.IsValid()) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<SpecularBrdf>(*reflectance, fresnel);
}

const Bxdf* MakeSpecularBtdf(BxdfAllocator& bxdf_allocator,
                             const Reflector* transmittance,
                             geometric_t eta_incident,
                             geometric_t eta_transmitted);

const Bxdf* MakeSpecularBxdf(BxdfAllocator& bxdf_allocator,
                             const Reflector* reflectance,
                             const Reflector* transmittance,
                             geometric_t eta_incident,
                             geometric_t eta_transmitted);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_SPECULAR_BXDF_
