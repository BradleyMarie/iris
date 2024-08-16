#ifndef _IRIS_BXDFS_OREN_NAYAR_BXDF_
#define _IRIS_BXDFS_OREN_NAYAR_BXDF_

#include "iris/bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

class OrenNayarBrdf final : public Bxdf {
 public:
  OrenNayarBrdf(const Reflector& reflector, visual_t sigma);

  bool IsDiffuse() const override;

  std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                      const Vector& surface_normal,
                                      Sampler& sampler) const override;

  std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const override;

  std::optional<visual_t> Pdf(const Vector& incoming, const Vector& outgoing,
                              const Vector& surface_normal,
                              const Bxdf* sample_source,
                              Hemisphere hemisphere) const override;

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               const Bxdf* sample_source, Hemisphere hemisphere,
                               SpectralAllocator& allocator) const override;

 private:
  const Reflector& reflector_;
  const visual_t a_, b_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_OREN_NAYAR_BXDF_