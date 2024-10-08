#include "iris/bxdfs/attenuated_bxdf.h"

#include <algorithm>
#include <cassert>

#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace {

class AttenuatedBxdf final : public Bxdf {
 public:
  AttenuatedBxdf(const Bxdf& bxdf, visual_t attenuation) noexcept
      : bxdf_(bxdf), attenuation_(attenuation) {
    assert(attenuation > static_cast<visual_t>(0.0));
    assert(attenuation < static_cast<visual_t>(1.0));
  }

  bool IsDiffuse(visual_t* diffuse_pdf) const override {
    return bxdf_.IsDiffuse(diffuse_pdf);
  }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override {
    return bxdf_.SampleDiffuse(incoming, surface_normal, sampler);
  }

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override {
    return bxdf_.Sample(incoming, differentials, surface_normal, sampler);
  }

  visual_t Pdf(const Vector& incoming, const Vector& outgoing,
               const Vector& surface_normal,
               Hemisphere hemisphere) const override {
    return bxdf_.Pdf(incoming, outgoing, surface_normal, hemisphere);
  }

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override {
    const Reflector* reflectance =
        bxdf_.Reflectance(incoming, outgoing, hemisphere, allocator);
    return allocator.UnboundedScale(reflectance, attenuation_);
  }

 private:
  const Bxdf& bxdf_;
  const visual_t attenuation_;
};

}  // namespace

const Bxdf* CreateAttenuatedBxdf(BxdfAllocator& allocator, const Bxdf* bxdf,
                                 visual_t attenuation) {
  if (bxdf == nullptr || attenuation <= static_cast<visual_t>(0.0)) {
    return nullptr;
  }

  if (attenuation >= static_cast<visual_t>(1.0)) {
    return bxdf;
  }

  return &allocator.Allocate<AttenuatedBxdf>(*bxdf, attenuation);
}

}  // namespace bxdfs
}  // namespace iris