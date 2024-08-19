#ifndef _IRIS_BXDF_
#define _IRIS_BXDF_

#include <optional>

#include "iris/float.h"
#include "iris/reflector.h"
#include "iris/sampler.h"
#include "iris/spectral_allocator.h"
#include "iris/vector.h"

namespace iris {

class Bxdf {
 public:
  virtual bool IsDiffuse(visual_t* diffuse_pdf = nullptr) const = 0;

  virtual std::optional<Vector> SampleDiffuse(const Vector& incoming,
                                              const Vector& surface_normal,
                                              Sampler& sampler) const = 0;

  struct Differentials {
    const Vector dx;
    const Vector dy;
  };

  struct SampleResult {
    const Vector direction;
    const std::optional<Differentials> differentials;
    const Bxdf* bxdf_override = nullptr;
    const visual_t pdf_weight = static_cast<visual_t>(1.0);
  };

  virtual std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      const Vector& surface_normal, Sampler& sampler) const = 0;

  enum class Hemisphere {
    BRDF,
    BTDF,
  };

  virtual visual_t Pdf(const Vector& incoming, const Vector& outgoing,
                       const Vector& surface_normal,
                       Hemisphere hemisphere) const = 0;

  virtual const Reflector* Reflectance(const Vector& incoming,
                                       const Vector& outgoing,
                                       Hemisphere hemisphere,
                                       SpectralAllocator& allocator) const = 0;

 protected:
  ~Bxdf() = default;
};

}  // namespace iris

#endif  // _IRIS_BXDF_