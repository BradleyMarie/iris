#include "iris/integrators/internal/russian_roulette.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace iris {
namespace integrators {
namespace internal {

RussianRoulette::RussianRoulette(
    visual_t maximum_continue_probability,
    visual_t always_continue_path_throughput) noexcept
    : maximum_continue_probability_(maximum_continue_probability),
      always_continue_path_throughput_(always_continue_path_throughput) {
  assert(0.0 <= maximum_continue_probability &&
         maximum_continue_probability <= 1.0);
  assert(0.0 <= always_continue_path_throughput);
}

std::optional<visual_t> RussianRoulette::Evaluate(
    Random& rng, visual_t path_throughput) const {
  if (always_continue_path_throughput_ <= path_throughput) {
    rng.DiscardVisual(1);
    return static_cast<visual_t>(1.0);
  }

  visual_t continue_probability =
      std::clamp(path_throughput, static_cast<visual_t>(0.0),
                 maximum_continue_probability_);
  if (continue_probability < rng.NextVisual()) {
    return std::nullopt;
  }

  return continue_probability;
}

}  // namespace internal
}  // namespace integrators
}  // namespace iris