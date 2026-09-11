#ifndef _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_SEQUENCE_
#define _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_SEQUENCE_

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/random_bitstream.h"

namespace iris {
namespace image_samplers {
namespace internal {

class LowDiscrepancySequence {
 public:
  virtual void Permute(RandomBitstream& rng) {}

  virtual bool Start(std::pair<size_t, size_t> image_dimensions,
                     std::pair<size_t, size_t> pixel,
                     unsigned sample_index) = 0;

  virtual std::optional<geometric_t> Next() = 0;
  virtual void Discard(size_t num_to_discard) = 0;

  virtual visual_t SampleWeight(uint32_t desired_num_samples) const = 0;

  virtual std::unique_ptr<LowDiscrepancySequence> Duplicate() = 0;

  virtual ~LowDiscrepancySequence() = default;
};

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_SEQUENCE_
