#ifndef _IRIS_BXDFS_INTERNAL_DIFFUSE_BXDF_
#define _IRIS_BXDFS_INTERNAL_DIFFUSE_BXDF_

#include "iris/bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {
namespace internal {

class DiffuseBxdf : public Bxdf {
 public:
  bool IsDiffuse(visual_t* diffuse_pdf = nullptr) const override final {
    if (diffuse_pdf) {
      *diffuse_pdf = static_cast<visual_t>(1.0);
    }

    return true;
  }

  std::variant<std::monostate, DiffuseSample, SpecularSample> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler,
      SpectralAllocator& allocator) const override final {
    std::optional<Vector> direction =
        SampleDiffuse(incoming, surface_normal, sampler);
    if (!direction) {
      return std::monostate();
    }

    return DiffuseSample{*direction};
  }

 protected:
  ~DiffuseBxdf() = default;
};

}  // namespace internal
}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_INTERNAL_DIFFUSE_BXDF_
