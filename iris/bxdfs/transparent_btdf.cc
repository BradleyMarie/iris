#include "iris/bxdfs/transparent_btdf.h"

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/helpers/specular_bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

class TransparentBtdf final : public helpers::SpecularBxdf {
 public:
  TransparentBtdf(const Reflector& transmittance) noexcept
      : transmittance_(transmittance) {}

  std::optional<Bxdf::SpecularSample> SampleSpecular(
      const Vector& incoming,
      const std::optional<Bxdf::Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override;

 private:
  const Reflector& transmittance_;
};

std::optional<Bxdf::SpecularSample> TransparentBtdf::SampleSpecular(
    const Vector& incoming, const std::optional<Differentials>& differentials,
    const Vector& surface_normal, Sampler& sampler,
    SpectralAllocator& allocator) const {
  return Bxdf::SpecularSample{Hemisphere::BTDF, incoming, &transmittance_,
                              differentials, static_cast<visual_t>(1.0)};
}

}  // namespace

const Bxdf* MakeTransparentBtdf(BxdfAllocator& bxdf_allocator,
                                const Reflector* transmittance) {
  if (!transmittance) {
    return nullptr;
  }

  return &bxdf_allocator.Allocate<TransparentBtdf>(*transmittance);
}

}  // namespace bxdfs
}  // namespace iris
