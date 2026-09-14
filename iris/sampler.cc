#include "iris/sampler.h"

#undef NDEBUG  // Enable assertions at runtime
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numbers>
#include <utility>
#include <vector>

#include "iris/float.h"

namespace iris {
namespace {

const static geometric_t kMinValue = std::nextafter(
    static_cast<geometric_t>(-1.0), static_cast<geometric_t>(1.0));
const static geometric_t kMaxValue = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(-1.0));

std::pair<geometric_t, geometric_t> NextGeometric(RandomBitstream& visual_rng,
                                                  Random* geometric_rng) {
  geometric_t first, second;

  if (geometric_rng) {
    first = geometric_rng->Next();
    assert(first >= static_cast<geometric_t>(0.0) &&
           first <= static_cast<geometric_t>(1.0));
    second = geometric_rng->Next();
    assert(second >= static_cast<geometric_t>(0.0) &&
           second <= static_cast<geometric_t>(1.0));
  } else {
    first = visual_rng.NextGeometric();
    second = visual_rng.NextGeometric();
  }

  return {first, second};
}

}  // namespace

Sampler::Sampler(RandomBitstream& visual_rng, Random* geometric_rng,
                 std::vector<visual_t>& visual_storage,
                 std::vector<std::pair<geometric_t, geometric_t>>&
                     geometric_storage) noexcept
    : Sampler(visual_rng, geometric_rng, visual_storage, geometric_storage, 0,
              std::numeric_limits<size_t>::max(), 0,
              std::numeric_limits<size_t>::max()) {
  visual_storage.clear();
  geometric_storage.clear();
}

Sampler Sampler::Claim(size_t visual_samples, size_t geometric_samples) {
  assert(visual_samples <= visual_indices_[1] - visual_indices_[0]);
  assert(geometric_samples <= geometric_indices_[1] - geometric_indices_[0]);

  size_t start_visual = visual_indices_[0];
  for (size_t i = 0; i < visual_samples; i++) {
    NextLinear1D();
  }

  size_t start_geometric = geometric_indices_[0];
  for (size_t i = 0; i < geometric_samples; i++) {
    NextLinear2D();
  }

  return Sampler(visual_rng_, geometric_rng_, visual_storage_,
                 geometric_storage_, start_visual, visual_indices_[0],
                 start_geometric, geometric_indices_[0]);
}

Sampler Sampler::Clone() const {
  return Sampler(visual_rng_, geometric_rng_, visual_storage_,
                 geometric_storage_, visual_indices_[0], visual_indices_[1],
                 geometric_indices_[0], geometric_indices_[1]);
}

visual_t Sampler::NextLinear1D() {
  assert(visual_indices_[0] < visual_indices_[1]);

  if (visual_indices_[0] == visual_storage_.size()) {
    visual_storage_.push_back(visual_rng_.NextVisual());
  }

  return visual_storage_[visual_indices_[0]++];
}

size_t Sampler::NextIndex1D(size_t size) {
  assert(size != 0 && size < (1u << std::numeric_limits<visual_t>::digits));
  size_t result = static_cast<visual_t>(size) * NextLinear1D();
  return std::min(result, size - 1u);
}

std::pair<geometric_t, geometric_t> Sampler::NextLinear2D() {
  assert(geometric_indices_[0] < geometric_indices_[1]);

  if (geometric_indices_[0] == geometric_storage_.size()) {
    geometric_storage_.emplace_back(NextGeometric(visual_rng_, geometric_rng_));
  }

  return geometric_storage_[geometric_indices_[0]++];
}

std::pair<geometric_t, geometric_t> Sampler::NextPolar() {
  std::pair<geometric_t, geometric_t> sample = NextLinear2D();

  geometric_t u = static_cast<geometric_t>(2.0) * sample.first -
                  static_cast<geometric_t>(1.0);
  u = std::clamp(u, kMinValue, kMaxValue);

  geometric_t v = static_cast<geometric_t>(2.0) * sample.second -
                  static_cast<geometric_t>(1.0);
  v = std::clamp(v, kMinValue, kMaxValue);

  if (u == static_cast<geometric_t>(0.0) &&
      v == static_cast<geometric_t>(0.0)) {
    return {static_cast<visual_t>(0.0), static_cast<visual_t>(0.0)};
  }

  geometric_t theta, radius;
  if (std::abs(u) > std::abs(v)) {
    radius = u;
    theta = static_cast<geometric_t>(std::numbers::pi / 4.0) * (v / u);
  } else {
    radius = v;
    theta = static_cast<geometric_t>(std::numbers::pi / 2.0) -
            static_cast<geometric_t>(std::numbers::pi / 4.0) * (u / v);
  }

  return {radius, theta};
}

Sampler::Sampler(
    RandomBitstream& visual_rng, Random* geometric_rng,
    std::vector<visual_t>& visual_storage,
    std::vector<std::pair<geometric_t, geometric_t>>& geometric_storage,
    size_t visual_start, size_t visual_end, size_t geometric_start,
    size_t geometric_end) noexcept
    : visual_rng_(visual_rng),
      geometric_rng_(geometric_rng),
      visual_storage_(visual_storage),
      geometric_storage_(geometric_storage),
      visual_indices_{visual_start, visual_end},
      geometric_indices_{geometric_start, geometric_end} {}

}  // namespace iris
