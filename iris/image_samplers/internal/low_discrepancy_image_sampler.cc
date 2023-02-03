#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"

namespace iris {
namespace image_samplers {
namespace internal {

void LowDiscrepancyImageSampler::StartPixel(
    std::pair<size_t, size_t> image_dimensions,
    std::pair<size_t, size_t> pixel) {
  image_dimensions_ = image_dimensions;
  pixel_ = pixel;
  sample_index_ = 0;
}

std::optional<ImageSampler::Sample> LowDiscrepancyImageSampler::NextSample(
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

std::unique_ptr<ImageSampler> LowDiscrepancyImageSampler::Replicate() const {
  return std::make_unique<LowDiscrepancyImageSampler>(
      sequence_->Duplicate(), desired_samples_per_pixel_);
}

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris