#ifndef _IRIS_BXDFS_LAMBERTIAN_BXDF_
#define _IRIS_BXDFS_LAMBERTIAN_BXDF_

#include "iris/bxdf.h"
#include "iris/float.h"
#include "iris/random.h"
#include "iris/reflector.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

class LambertianBrdf final : public Bxdf {
 public:
  LambertianBrdf(const Reflector& reflector) noexcept : reflector_(reflector) {}

  Vector Sample(const Vector& incoming, Random& rng) const override;

  visual_t DiffusePdf(const Vector& incoming,
                      const Vector& outgoing) const override;

  std::optional<visual_t> SamplePdf(const Vector& incoming,
                                    const Vector& outgoing) const override;

  const Reflector* Reflectance(const Vector& incoming, const Vector& outgoing,
                               Type type,
                               SpectralAllocator& allocator) const override;

 private:
  const Reflector& reflector_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_LAMBERTIAN_BXDF_