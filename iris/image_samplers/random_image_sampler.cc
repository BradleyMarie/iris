#include "iris/image_samplers/random_image_sampler.h"

namespace iris {
namespace image_samplers {

RandomImageSampler::RandomImageSampler(uint32_t samples_per_pixel) noexcept
    : samples_per_pixel_(samples_per_pixel),
      sample_weight_(std::pow(samples_per_pixel, -1.0)),
      pixel_start_x_(0.0),
      pixel_start_y_(0.0),
      subpixel_size_x_(0.0),
      subpixel_size_y_(0.0),
      sample_index_(0) {}

void RandomImageSampler::StartPixel(std::pair<size_t, size_t> image_dimensions,
                                    std::pair<size_t, size_t> pixel) {
  pixel_start_x_ = static_cast<geometric_t>(pixel.second) /
                   static_cast<geometric_t>(image_dimensions.second);
  pixel_start_y_ = static_cast<geometric_t>(pixel.first) /
                   static_cast<geometric_t>(image_dimensions.first);
  pixel_size_x_ = static_cast<geometric_t>(1.0) /
                  static_cast<geometric_t>(image_dimensions.second);
  pixel_size_y_ = static_cast<geometric_t>(1.0) /
                  static_cast<geometric_t>(image_dimensions.first);
  subpixel_size_x_ = pixel_size_x_;
  subpixel_size_y_ = pixel_size_y_;

  if (samples_per_pixel_ != 0) {
    geometric_t samples_per_dimension =
        std::sqrt(static_cast<double>(samples_per_pixel_));
    subpixel_size_x_ /= samples_per_dimension;
    subpixel_size_y_ /= samples_per_dimension;
  }

  sample_index_ = 0;
}

std::optional<ImageSampler::Sample> RandomImageSampler::NextSample(
    bool sample_lens, Random& rng) {
  if (sample_index_ == samples_per_pixel_) {
    return std::nullopt;
  }

  geometric_t image_u =
      std::fma(rng.NextGeometric(), pixel_size_x_, pixel_start_x_);
  geometric_t image_v =
      std::fma(rng.NextGeometric(), pixel_size_y_, pixel_start_y_);
  geometric_t image_u_dx = image_u + subpixel_size_x_;
  geometric_t image_v_dv = image_v + subpixel_size_y_;
  sample_index_ += 1;

  std::optional<std::array<geometric_t, 2>> lens_uv;
  if (sample_lens) {
    lens_uv.emplace(
        std::array<geometric_t, 2>{rng.NextGeometric(), rng.NextGeometric()});
  }

  return ImageSampler::Sample{{image_u, image_v},
                              {{image_u_dx, image_v_dv}},
                              lens_uv,
                              sample_weight_,
                              rng};
}

std::unique_ptr<ImageSampler> RandomImageSampler::Replicate() const {
  return std::make_unique<RandomImageSampler>(samples_per_pixel_);
}

}  // namespace image_samplers
}  // namespace iris