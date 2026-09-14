#include "iris/random_bitstream.h"

#include <cstdint>
#include <random>

#include "iris/float.h"

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

geometric_t RandomBitstream::NextGeometric() {
  Generator rng(*this);
  std::uniform_real_distribution<geometric> distribution(
      static_cast<geometric>(0.0), static_cast<geometric>(1.0));
  return distribution(rng);
}

visual_t RandomBitstream::NextVisual() {
  Generator rng(*this);
  std::uniform_real_distribution<visual> distribution(static_cast<visual>(0.0),
                                                      static_cast<visual>(1.0));
  return distribution(rng);
}

}  // namespace iris
