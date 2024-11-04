#ifndef _IRIS_BXDFS_HELPERS_SPECULAR_BXDF_
#define _IRIS_BXDFS_HELPERS_SPECULAR_BXDF_

#include "iris/bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace helpers {

class SpecularBxdf : public Bxdf {
 public:
  bool IsDiffuse(visual_t* diffuse_pdf) const override final {
    if (diffuse_pdf) {
      *diffuse_pdf = static_cast<visual_t>(0.0);
    }

    return false;
  }

  std::variant<std::monostate, DiffuseSample, SpecularSample> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override final {
    std::optional<SpecularSample> result = SampleSpecular(
        incoming, differentials, surface_normal, sampler, allocator);
    if (!result) {
      return std::monostate();
    }

    return *result;
  }

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override final {
    return std::nullopt;
  }

  visual_t PdfDiffuse(const Vector& incoming, const Vector& outgoing,
                      const Vector& surface_normal,
                      Hemisphere hemisphere) const override final {
    return static_cast<visual_t>(0.0);
  }

  const Reflector* ReflectanceDiffuse(
      const Vector& incoming, const Vector& outgoing, Hemisphere hemisphere,
      SpectralAllocator& allocator) const override final {
    return nullptr;
  }

  virtual std::optional<SpecularSample> SampleSpecular(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const = 0;

 protected:
  ~SpecularBxdf() = default;
};

}  // namespace helpers
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_HELPERS_SPECULAR_BXDF_