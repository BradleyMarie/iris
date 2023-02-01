#include "iris/image_samplers/halton_sampler.h"

#include "iris/image_samplers/internal/halton_sequence.h"

namespace iris {
namespace image_samplers {

HaltonImageSampler::HaltonImageSampler(uint32_t samples_per_pixel)
    : HaltonImageSampler(std::make_unique<internal::HaltonSequence>(),
                         samples_per_pixel_) {}

ImageSampler::Sample HaltonImageSampler::SamplePixel(
    std::pair<size_t, size_t> image_dimensions, std::pair<size_t, size_t> pixel,
    uint32_t sample_index, bool sample_lens, Random& rng) {
  sequence_->Start(image_dimensions, pixel, sample_index);

  geometric_t image_u = rng.NextGeometric();
  geometric_t image_v = rng.NextGeometric();

  if (!sample_lens) {
    return {{image_u, image_v}, {}, *sequence_};
  }

  return {{image_u, image_v},
          {{rng.NextGeometric(), rng.NextGeometric()}},
          *sequence_};
}

uint32_t HaltonImageSampler::SamplesPerPixel() const {
  return samples_per_pixel_;
}

std::unique_ptr<ImageSampler> HaltonImageSampler::Duplicate() const {
  return std::make_unique<HaltonImageSampler>(sequence_->Duplicate(),
                                              samples_per_pixel_);
}

}  // namespace image_samplers
}  // namespace iris