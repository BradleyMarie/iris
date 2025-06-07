#include "iris/integrators/internal/russian_roulette.h"

#include <algorithm>
#include <cmath>
#include <optional>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace integrators {
namespace internal {

RussianRoulette::RussianRoulette(
    visual_t maximum_continue_probability,
    visual_t always_continue_path_throughput) noexcept
    : maximum_continue_probability_(std::max(
          static_cast<visual_t>(0.0),
          std::min(maximum_continue_probability, static_cast<visual_t>(1.0)))),
      always_continue_path_throughput_(std::max(
          static_cast<visual_t>(0.0), always_continue_path_throughput)) {}

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
