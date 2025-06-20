#ifndef _IRIS_INTEGRATORS_INTERNAL_RUSSIAN_ROULETTE_
#define _IRIS_INTEGRATORS_INTERNAL_RUSSIAN_ROULETTE_

#include <optional>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace integrators {
namespace internal {

class RussianRoulette final {
 public:
  RussianRoulette(visual_t maximum_continue_probability,
                  visual_t always_continue_path_throughput) noexcept;

  std::optional<visual_t> Evaluate(Random& rng, visual_t path_throughput) const;

 private:
  visual_t maximum_continue_probability_;
  visual_t always_continue_path_throughput_;
};

}  // namespace internal
}  // namespace integrators
}  // namespace iris

#endif  // _IRIS_INTEGRATORS_INTERNAL_RUSSIAN_ROULETTE_
