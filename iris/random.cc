#include "iris/random.h"

namespace iris {

void Random::DiscardGeometric(size_t num_to_discard) {
  for (size_t i = 0; i < num_to_discard; i++) {
    NextGeometric();
  }
}

}  // namespace iris
