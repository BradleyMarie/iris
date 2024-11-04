#ifndef _IRIS_BXDFS_ASHIKHMIN_SHIRLEY_BRDF_
#define _IRIS_BXDFS_ASHIKHMIN_SHIRLEY_BRDF_

#include <concepts>

#include "iris/bxdf.h"
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

class AshikhminShirleyBrdf final : public helpers::DiffuseBxdf {
 public:
  AshikhminShirleyBrdf(const Reflector& diffuse, const Reflector& specular,
                       const MicrofacetDistribution& distribution) noexcept
      : diffuse_(diffuse), specular_(specular), distribution_(distribution) {}

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
  const Reflector& diffuse_;
  const Reflector& specular_;
  const MicrofacetDistribution& distribution_;
};

}  // namespace internal

template <typename M>
  requires std::derived_from<M, MicrofacetDistribution>
class AshikhminShirleyBrdf final : public helpers::DiffuseBxdf {
 public:
  AshikhminShirleyBrdf(const Reflector& diffuse, const Reflector& specular,
                       const M& distribution) noexcept
      : distribution_(distribution), impl_(diffuse, specular, distribution_) {}

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
    return impl_.ReflectanceDiffuse(incoming, outgoing, hemisphere, allocator);
  }

 private:
  const M distribution_;
  const internal::AshikhminShirleyBrdf impl_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_ASHIKHMIN_SHIRLEY_BRDF_