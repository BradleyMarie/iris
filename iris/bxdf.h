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
  struct Differentials {
    const Vector dx;
    const Vector dy;
  };

  struct SampleResult {
    const Vector direction;
    const std::optional<Differentials> differentials;
    const Bxdf* sample_source;
  };

  virtual std::optional<SampleResult> Sample(
      const Vector& incoming, const std::optional<Differentials>& differentials,
      Sampler& sampler) const = 0;

  enum class Hemisphere {
    BRDF,
    BTDF,
  };

  virtual std::optional<visual_t> Pdf(const Vector& incoming,
                                      const Vector& outgoing,
                                      const Bxdf* sample_source,
                                      Hemisphere hemisphere) const = 0;

  virtual const Reflector* Reflectance(const Vector& incoming,
                                       const Vector& outgoing,
                                       const Bxdf* sample_source,
                                       Hemisphere hemisphere,
                                       SpectralAllocator& allocator) const = 0;

 protected:
  ~Bxdf() = default;
};

}  // namespace iris

#endif  // _IRIS_BXDF_