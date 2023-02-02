#include "iris/image_samplers/halton_sampler.h"

#include "iris/image_samplers/internal/halton_sequence.h"

namespace iris {
namespace image_samplers {

HaltonImageSampler::HaltonImageSampler(uint32_t desired_samples_per_pixel)
    : HaltonImageSampler(std::make_unique<internal::HaltonSequence>(),
                         desired_samples_per_pixel_) {}

void HaltonImageSampler::StartPixel(std::pair<size_t, size_t> image_dimensions,
                                    std::pair<size_t, size_t> pixel) {
  image_dimensions_ = image_dimensions;
  pixel_ = pixel;
  sample_index_ = 0;
}

std::optional<ImageSampler::Sample> HaltonImageSampler::NextSample(
    bool sample_lens, Random& rng) {
  if (!sequence_->Start(image_dimensions_, pixel_, sample_index_)) {
    return std::nullopt;
  }

  geometric_t image_u = sequence_->NextGeometric();
  geometric_t image_v = sequence_->NextGeometric();
  visual_t sample_weight = sequence_->SampleWeight(desired_samples_per_pixel_);

  if (!sample_lens) {
    return ImageSampler::Sample{
        {image_u, image_v}, {}, sample_weight, *sequence_};
  }

  return ImageSampler::Sample{
      {image_u, image_v},
      {{sequence_->NextGeometric(), sequence_->NextGeometric()}},
      sample_weight,
      *sequence_};
}

std::unique_ptr<ImageSampler> HaltonImageSampler::Replicate() const {
  return std::make_unique<HaltonImageSampler>(sequence_->Duplicate(),
                                              desired_samples_per_pixel_);
}

}  // namespace image_samplers
}  // namespace iris