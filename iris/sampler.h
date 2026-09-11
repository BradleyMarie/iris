#ifndef _IRIS_SAMPLER_
#define _IRIS_SAMPLER_

#include <utility>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {

class Sampler {
 public:
  Sampler(Random& rng) noexcept : rng_(rng), samples_1d_(2), samples_2d_(1) {}

  Sampler(Sampler&& from)
      : rng_(from.rng_),
        samples_1d_(from.samples_1d_),
        samples_2d_(from.samples_2d_) {
    from.samples_1d_ = 0;
    from.samples_2d_ = 0;
  }

  Sampler(const Sampler&) = delete;
  Sampler& operator=(const Sampler&) = delete;
  Sampler& operator=(Sampler&& from) = delete;

  ~Sampler() {
    if (samples_2d_) {
      rng_.DiscardGeometric(2);
    }
  }

  visual_t NextLinear1D();
  size_t NextIndex1D(size_t max_value);

  std::pair<geometric_t, geometric_t> NextLinear2D();
  std::pair<geometric_t, geometric_t> NextPolar();

 private:
  Random& rng_;
  unsigned samples_1d_;
  unsigned samples_2d_;
};

}  // namespace iris

#endif  // _IRIS_SAMPLER_
