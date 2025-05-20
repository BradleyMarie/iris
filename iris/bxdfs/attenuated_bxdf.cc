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

  std::variant<std::monostate, DiffuseSample, SpecularSample> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override {
    auto sample = bxdf_.Sample(incoming, differentials, surface_normal, sampler,
                               allocator);

    if (SpecularSample* specular_sample = std::get_if<SpecularSample>(&sample);
        specular_sample != nullptr) {
      specular_sample->reflectance =
          allocator.UnboundedScale(specular_sample->reflectance, attenuation_);
    }

    return sample;
  }

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override {
    return bxdf_.PdfDiffuse(incoming, outgoing, surface_normal, hemisphere);
  }

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override {
    const Reflector* reflectance =
        bxdf_.ReflectanceDiffuse(incoming, outgoing, hemisphere, allocator);
    return allocator.UnboundedScale(reflectance, attenuation_);
  }

 private:
  const Bxdf& bxdf_;
  const visual_t attenuation_;
};

}  // namespace

const Bxdf* MakeAttenuatedBxdf(BxdfAllocator& allocator, const Bxdf* bxdf,
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
