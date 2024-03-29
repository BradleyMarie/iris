#include "iris/integrators/internal/russian_roulette.h"

#include <cassert>
#include <cmath>

namespace iris {
namespace integrators {
namespace internal {

RussianRoulette::RussianRoulette(
    visual maximum_continue_probability,
    visual always_continue_path_throughput) noexcept
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

  visual continue_probability =
      std::max(static_cast<visual>(0.0),
               std::min(path_throughput, static_cast<visual>(1.0)));

  visual_t termination_cutoff =
      std::min(maximum_continue_probability_, continue_probability);

  auto roulette_sample = rng.NextVisual();
  if (termination_cutoff < roulette_sample) {
    return std::nullopt;
  }

  return termination_cutoff;
}

}  // namespace internal
}  // namespace integrators
}  // namespace iris