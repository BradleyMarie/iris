#ifndef _IRIS_IMAGE_SAMPLERS_HALTON_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_HALTON_IMAGE_SAMPLER_

#include <array>
#include <memory>
#include <optional>
#include <utility>

#include "iris/image_sampler.h"

namespace iris {
namespace image_samplers {
namespace internal {
class LowDiscrepancySequence;
}  // namespace internal

class HaltonImageSampler final : public ImageSampler {
 public:
  HaltonImageSampler(uint32_t samples_per_pixel);

  Sample SamplePixel(std::pair<size_t, size_t> image_dimensions,
                     std::pair<size_t, size_t> pixel, uint32_t sample_index,
                     bool sample_lens, Random& rng) override;
  uint32_t SamplesPerPixel() const override;

  std::unique_ptr<ImageSampler> Duplicate() const override;

 private:
  HaltonImageSampler(std::unique_ptr<internal::LowDiscrepancySequence> sequence,
                     uint32_t samples_per_pixel)
      : sequence_(std::move(sequence)), samples_per_pixel_(samples_per_pixel) {}

  const std::unique_ptr<internal::LowDiscrepancySequence> sequence_;
  const uint32_t samples_per_pixel_;
};

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_HALTON_IMAGE_SAMPLER_