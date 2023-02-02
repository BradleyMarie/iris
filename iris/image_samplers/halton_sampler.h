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
  HaltonImageSampler(uint32_t desired_samples_per_pixel);

  void StartPixel(std::pair<size_t, size_t> image_dimensions,
                  std::pair<size_t, size_t> pixel) override;
  std::optional<Sample> NextSample(bool sample_lens, Random& rng) override;

  std::unique_ptr<ImageSampler> Replicate() const override;

 private:
  HaltonImageSampler(std::unique_ptr<internal::LowDiscrepancySequence> sequence,
                     uint32_t desired_samples_per_pixel)
      : sequence_(std::move(sequence)),
        desired_samples_per_pixel_(desired_samples_per_pixel),
        image_dimensions_(0, 0),
        pixel_(0, 0),
        sample_index_(0) {}

  const std::unique_ptr<internal::LowDiscrepancySequence> sequence_;
  const uint32_t desired_samples_per_pixel_;
  std::pair<size_t, size_t> image_dimensions_;
  std::pair<size_t, size_t> pixel_;
  unsigned sample_index_;
};

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_HALTON_IMAGE_SAMPLER_