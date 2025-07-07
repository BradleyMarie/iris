#ifndef _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_IMAGE_SAMPLER_

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_sampler.h"
#include "iris/image_samplers/internal/low_discrepancy_sequence.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {
namespace internal {

class LowDiscrepancyImageSampler final : public ImageSampler {
 public:
  LowDiscrepancyImageSampler(std::unique_ptr<LowDiscrepancySequence> sequence,
                             uint32_t desired_samples_per_pixel)
      : sequence_(std::move(sequence)),
        desired_samples_per_pixel_(desired_samples_per_pixel),
        image_dimensions_(0, 0),
        pixel_(0, 0),
        subpixel_size_x_(0.0),
        subpixel_size_y_(0.0),
        sample_index_(0) {}

  void StartPixel(std::pair<size_t, size_t> image_dimensions,
                  std::pair<size_t, size_t> pixel, Random& rng) override;
  std::optional<Sample> NextSample(bool sample_lens, Random& rng) override;

  std::unique_ptr<ImageSampler> Replicate() const override;

 private:
  const std::unique_ptr<LowDiscrepancySequence> sequence_;
  const uint32_t desired_samples_per_pixel_;
  std::pair<size_t, size_t> image_dimensions_;
  std::pair<size_t, size_t> pixel_;
  geometric_t subpixel_size_x_;
  geometric_t subpixel_size_y_;
  unsigned sample_index_;
};

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_INTERNAL_LOW_DISCREPANCY_IMAGE_SAMPLER_
