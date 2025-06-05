#include "iris/random/mersenne_twister_random.h"

#include <cassert>
#include <limits>
#include <memory>
#include <random>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace random {
namespace {

class MersenneTwisterRandom final : public Random {
 public:
  MersenneTwisterRandom(
      std::mt19937::result_type default_seed = std::mt19937::default_seed)
      : rng_(default_seed) {}

  size_t NextIndex(size_t size) override;
  geometric NextGeometric() override;
  visual NextVisual() override;

  std::unique_ptr<Random> Replicate() override;

 private:
  std::mt19937 rng_;
};

size_t MersenneTwisterRandom::NextIndex(size_t size) {
  assert(size != 0 && size <= std::numeric_limits<uint32_t>::max());
  std::uniform_int_distribution<uint32_t> distribution(0, size - 1u);
  return distribution(rng_);
}

geometric MersenneTwisterRandom::NextGeometric() {
  return std::generate_canonical<geometric,
                                 std::numeric_limits<geometric>::digits>(rng_);
}

visual MersenneTwisterRandom::NextVisual() {
  return std::generate_canonical<visual, std::numeric_limits<visual>::digits>(
      rng_);
}

std::unique_ptr<Random> MersenneTwisterRandom::Replicate() {
  return std::make_unique<MersenneTwisterRandom>(rng_());
}

}  // namespace

std::unique_ptr<Random> MakeMersenneTwisterRandom() {
  return std::make_unique<MersenneTwisterRandom>();
}

}  // namespace random
}  // namespace iris
