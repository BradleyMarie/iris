#include "iris/random_bitstreams/mersenne_twister_random_bitstream.h"

#include <memory>
#include <random>

#include "iris/random_bitstream.h"

namespace iris {
namespace random_bitstreams {
namespace {

class MersenneTwisterRandom final : public RandomBitstream {
 public:
  MersenneTwisterRandom(
      std::mt19937::result_type seed = std::mt19937::default_seed)
      : rng_(seed) {}

  uint32_t Next() override;
  std::unique_ptr<RandomBitstream> Replicate() override;

 private:
  std::mt19937 rng_;
};

uint32_t MersenneTwisterRandom::Next() { return rng_(); }

std::unique_ptr<RandomBitstream> MersenneTwisterRandom::Replicate() {
  return std::make_unique<MersenneTwisterRandom>(rng_());
}

}  // namespace

std::unique_ptr<RandomBitstream> MakeMersenneTwisterRandomBitstream() {
  return std::make_unique<MersenneTwisterRandom>();
}

}  // namespace random_bitstreams
}  // namespace iris
