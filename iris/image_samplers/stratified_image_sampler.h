#ifndef _IRIS_IMAGE_SAMPLERS_STRATIFIED_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_STRATIFIED_IMAGE_SAMPLER_

#include <array>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_sampler.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {

class StratifiedImageSampler final : public ImageSampler {
 public:
  StratifiedImageSampler(uint16_t x_samples, uint16_t y_samples,
                         bool jittered) noexcept;

  void StartPixel(std::pair<size_t, size_t> image_dimensions,
                  std::pair<size_t, size_t> pixel) override;
  std::optional<Sample> NextSample(bool sample_lens, Random& rng) override;

  std::unique_ptr<ImageSampler> Replicate() const override;

 private:
  const uint16_t x_samples_;
  const uint16_t y_samples_;
  const bool jittered_;
  const visual_t sample_weight_;
  uint64_t num_subpixels_x_;
  uint64_t num_subpixels_y_;
  uint64_t subpixel_x_;
  uint64_t subpixel_y_;
  uint32_t sample_index_;
};

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_STRATIFIED_IMAGE_SAMPLER_