#include "iris/testing/sampler.h"

#include <deque>
#include <utility>
#include <vector>

#include "iris/float.h"
#include "iris/random_bitstreams/mock_random_bitstream.h"
#include "iris/sampler.h"

namespace iris {
namespace testing {

Sampler MakeSampler(
    std::vector<visual_t> visual_samples,
    std::vector<std::pair<geometric_t, geometric_t>> geometric_samples) {
  thread_local random_bitstreams::MockRandomBitstream rng;
  thread_local std::deque<std::vector<visual_t>> vis;
  thread_local std::deque<std::vector<std::pair<geometric_t, geometric_t>>> geo;
  vis.emplace_back();
  geo.emplace_back();
  Sampler root_sampler(rng, nullptr, vis.back(), geo.back());
  vis.back() = visual_samples;
  geo.back() = geometric_samples;
  return root_sampler.Claim(visual_samples.size(), geometric_samples.size());
}

}  // namespace testing
}  // namespace iris
