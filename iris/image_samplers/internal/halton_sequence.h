#ifndef _IRIS_IMAGE_SAMPLERS_INTERNAL_HALTON_SEQUENCE_
#define _IRIS_IMAGE_SAMPLERS_INTERNAL_HALTON_SEQUENCE_

#include <memory>
#include <optional>

#include "iris/image_samplers/internal/low_discrepancy_sequence.h"
#include "third_party/gruenschloss/halton/halton_enum.h"
#include "third_party/gruenschloss/halton/halton_sampler.h"

namespace iris {
namespace image_samplers {
namespace internal {

class HaltonSequence final : public LowDiscrepancySequence {
 public:
  HaltonSequence();
  HaltonSequence(const HaltonSequence& to_copy) = default;

  bool Start(std::pair<size_t, size_t> image_dimensions,
             std::pair<size_t, size_t> pixel, unsigned sample_index) override;
  std::optional<geometric_t> Next() override;
  void Discard(size_t num_to_discard) override;

  visual_t SampleWeight(uint32_t desired_num_samples) const override;

  std::unique_ptr<LowDiscrepancySequence> Duplicate() override;

 private:
  std::shared_ptr<Halton_sampler> sampler_;
  std::pair<size_t, size_t> image_dimensions_;
  geometric_t x_offset_;
  geometric_t y_offset_;
  unsigned sample_index_;
  unsigned dimension_;
};

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_INTERNAL_HALTON_SEQUENCE_