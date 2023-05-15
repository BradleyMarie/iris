#ifndef _IRIS_IMAGE_SAMPLERS_RANDOM_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_RANDOM_IMAGE_SAMPLER_

#include <array>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_sampler.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {

class RandomImageSampler final : public ImageSampler {
 public:
  RandomImageSampler(uint32_t samples_per_pixel) noexcept;

  void StartPixel(std::pair<size_t, size_t> image_dimensions,
                  std::pair<size_t, size_t> pixel) override;
  std::optional<Sample> NextSample(bool sample_lens, Random& rng) override;

  std::unique_ptr<ImageSampler> Replicate() const override;

 private:
  const uint32_t samples_per_pixel_;
  const visual_t sample_weight_;
  geometric_t pixel_start_x_;
  geometric_t pixel_start_y_;
  geometric_t pixel_size_x_;
  geometric_t pixel_size_y_;
  geometric_t subpixel_size_x_;
  geometric_t subpixel_size_y_;
  uint32_t sample_index_;
};

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_RANDOM_IMAGE_SAMPLER_