#ifndef _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_SEQUENCE_
#define _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_SEQUENCE_

#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {
namespace internal {

class LowDiscrepancySequence : public Random {
 public:
  // Random Interface
  size_t NextIndex(size_t size) final;
  void DiscardIndex(size_t num_to_discard) final;

  geometric NextGeometric() final;
  void DiscardGeometric(size_t num_to_discard) final;

  visual NextVisual() final;
  void DiscardVisual(size_t num_to_discard) final;

  std::unique_ptr<Random> Replicate() final;

  // LowDiscrepancySequence Interface
  virtual void Permute(Random& rng) {}

  virtual bool Start(std::pair<size_t, size_t> image_dimensions,
                     std::pair<size_t, size_t> pixel,
                     uint32_t sample_index) = 0;

  virtual std::optional<geometric_t> Next() = 0;
  virtual void Discard(size_t num_to_discard) = 0;

  virtual std::unique_ptr<LowDiscrepancySequence> Duplicate() = 0;
};

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_SEQUENCE_