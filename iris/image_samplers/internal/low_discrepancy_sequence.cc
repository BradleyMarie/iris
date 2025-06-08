#include "iris/image_samplers/internal/low_discrepancy_sequence.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {
namespace internal {

size_t LowDiscrepancySequence::NextIndex(size_t size) {
  geometric_t as_geometric = static_cast<geometric_t>(size);
  as_geometric *= Next().value();
  return std::min(static_cast<size_t>(as_geometric), size - 1u);
}

void LowDiscrepancySequence::DiscardIndex(size_t num_to_discard) {
  Discard(num_to_discard);
}

geometric LowDiscrepancySequence::NextGeometric() { return Next().value(); }

void LowDiscrepancySequence::DiscardGeometric(size_t num_to_discard) {
  Discard(num_to_discard);
}

visual LowDiscrepancySequence::NextVisual() { return Next().value(); }

void LowDiscrepancySequence::DiscardVisual(size_t num_to_discard) {
  Discard(num_to_discard);
}

std::unique_ptr<Random> LowDiscrepancySequence::Replicate() {
  return Duplicate();
}

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris
