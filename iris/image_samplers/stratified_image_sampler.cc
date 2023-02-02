#include "iris/image_samplers/stratified_image_sampler.h"

#include <cmath>

namespace iris {
namespace image_samplers {

StratifiedImageSampler::StratifiedImageSampler(uint16_t x_samples,
                                               uint16_t y_samples,
                                               bool jittered) noexcept
    : x_samples_(x_samples),
      y_samples_(y_samples),
      jittered_(jittered),
      sample_weight_(std::pow(x_samples * y_samples, -1.0)),
      pixel_start_x_(0.0),
      pixel_start_y_(0.0),
      subpixel_size_x_(0.0),
      subpixel_size_y_(0.0),
      sample_index_(0) {}

void StratifiedImageSampler::StartPixel(
    std::pair<size_t, size_t> image_dimensions,
    std::pair<size_t, size_t> pixel) {
  pixel_start_x_ = static_cast<geometric_t>(pixel.second) /
                   static_cast<geometric_t>(image_dimensions.second);
  pixel_start_y_ = static_cast<geometric_t>(pixel.first) /
                   static_cast<geometric_t>(image_dimensions.first);
  subpixel_size_x_ = static_cast<geometric_t>(1.0) /
                     (static_cast<geometric_t>(image_dimensions.second) *
                      static_cast<geometric_t>(x_samples_));
  subpixel_size_y_ = static_cast<geometric_t>(1.0) /
                     (static_cast<geometric_t>(image_dimensions.first) *
                      static_cast<geometric_t>(y_samples_));
  sample_index_ = 0;
}

std::optional<ImageSampler::Sample> StratifiedImageSampler::NextSample(
    bool sample_lens, Random& rng) {
  if (sample_index_ == x_samples_ * y_samples_) {
    return std::nullopt;
  }

  uint16_t x_index = sample_index_ % x_samples_;
  uint16_t y_index = sample_index_ / x_samples_;
  sample_index_ += 1;

  geometric x_sample = jittered_ ? rng.NextGeometric() : 0.5;
  geometric y_sample = jittered_ ? rng.NextGeometric() : 0.5;

  geometric_t image_u = std::fma(static_cast<geometric_t>(x_index) + x_sample,
                                 subpixel_size_x_, pixel_start_x_);
  geometric_t image_v = std::fma(static_cast<geometric_t>(y_index) + y_sample,
                                 subpixel_size_y_, pixel_start_y_);

  if (!sample_lens) {
    return ImageSampler::Sample{{image_u, image_v}, {}, sample_weight_, rng};
  }

  return ImageSampler::Sample{{image_u, image_v},
                              {{rng.NextGeometric(), rng.NextGeometric()}},
                              sample_weight_,
                              rng};
}

std::unique_ptr<ImageSampler> StratifiedImageSampler::Replicate() const {
  return std::make_unique<StratifiedImageSampler>(x_samples_, y_samples_,
                                                  jittered_);
}

}  // namespace image_samplers
}  // namespace iris