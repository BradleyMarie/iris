#ifndef _IRIS_BXDFS_OREN_NAYAR_BXDF_
#define _IRIS_BXDFS_OREN_NAYAR_BXDF_

#include "iris/bxdf.h"
#include "iris/bxdfs/helpers/diffuse_bxdf.h"
#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {
namespace bxdfs {

class OrenNayarBrdf final : public helpers::DiffuseBxdf {
 public:
  OrenNayarBrdf(const Reflector& reflector, visual_t sigma);

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
  const Reflector& reflector_;
  const visual_t a_, b_;
};

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_OREN_NAYAR_BXDF_