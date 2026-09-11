#include "iris/random_bitstream.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <random>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace {

class Generator {
 public:
  Generator(RandomBitstream& rng) : rng_(rng) {}
  typedef uint32_t result_type;
  uint32_t operator()() { return rng_.Next(); }
  static constexpr uint32_t min() { return 0u; }
  static constexpr uint32_t max() { return 0xFFFFFFFFu; }

 private:
  RandomBitstream& rng_;
};

}  //  namespace

geometric RandomBitstream::NextGeometric() {
  Generator rng(*this);
  std::uniform_real_distribution<geometric> distribution(
      static_cast<geometric>(0.0), static_cast<geometric>(1.0));
  return distribution(rng);
}

void RandomBitstream::DiscardGeometric(size_t num_to_discard) {
  // Do nothing
}

visual RandomBitstream::NextVisual() {
  Generator rng(*this);
  std::uniform_real_distribution<visual> distribution(static_cast<visual>(0.0),
                                                      static_cast<visual>(1.0));
  return distribution(rng);
}

size_t RandomBitstream::NextIndex(size_t size) {
  assert(size != 0);

  Generator rng(*this);
  std::uniform_int_distribution<size_t> distribution(static_cast<size_t>(0u),
                                                     size - 1u);
  return distribution(rng);
}

}  // namespace iris
