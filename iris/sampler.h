#ifndef _IRIS_SAMPLER_
#define _IRIS_SAMPLER_

#include <optional>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {

class Sampler {
 public:
  Sampler(Random& rng) noexcept : rng_(rng), samples_(2) {}

  Sampler(Sampler&& from)
      : rng_(from.rng_), next_(from.next_), samples_(from.samples_) {
    from.samples_ = 0;
    from.next_.reset();
  }

  Sampler(const Sampler&) = delete;
  Sampler& operator=(const Sampler&) = delete;
  Sampler& operator=(Sampler&& from) = delete;

  ~Sampler() {
    if (samples_ != 0) {
      rng_.DiscardGeometric(samples_);
    }
  }

  size_t NextIndex(size_t max);
  geometric_t Next();

 private:
  std::optional<geometric_t> NextImpl();

  Random& rng_;
  std::optional<geometric_t> next_;
  unsigned samples_;
};

}  // namespace iris

#endif  // _IRIS_SAMPLER_
