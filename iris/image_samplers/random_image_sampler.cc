#include "iris/image_samplers/random_image_sampler.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_sampler.h"
#include "iris/random_bitstream.h"

namespace iris {
namespace image_samplers {
namespace {

class RandomImageSampler final : public ImageSampler {
 public:
  RandomImageSampler(uint32_t samples_per_pixel) noexcept;

  void StartPixel(std::pair<size_t, size_t> image_dimensions,
                  std::pair<size_t, size_t> pixel,
                  RandomBitstream& rng) override;
  std::optional<Sample> NextSample(bool sample_lens,
                                   RandomBitstream& rng) override;

  std::unique_ptr<ImageSampler> Replicate() const override;

 private:
  const uint32_t samples_per_pixel_;
  const visual_t sample_weight_;
  double num_pixels_x_;
  double num_pixels_y_;
  double pixel_x_;
  double pixel_y_;
  double subpixel_size_;
  uint32_t sample_index_;
};

RandomImageSampler::RandomImageSampler(uint32_t samples_per_pixel) noexcept
    : samples_per_pixel_(samples_per_pixel),
      sample_weight_(std::pow(samples_per_pixel, -1.0)),
      num_pixels_x_(0.0),
      num_pixels_y_(0.0),
      pixel_x_(0.0),
      pixel_y_(0.0),
      subpixel_size_(0.0),
      sample_index_(0) {}

void RandomImageSampler::StartPixel(std::pair<size_t, size_t> image_dimensions,
                                    std::pair<size_t, size_t> pixel,
                                    RandomBitstream& rng) {
  num_pixels_x_ = image_dimensions.second;
  num_pixels_y_ = image_dimensions.first;
  pixel_x_ = pixel.second;
  pixel_y_ = pixel.first;
  subpixel_size_ = std::sqrt(static_cast<double>(samples_per_pixel_));
  sample_index_ = 0;
}

std::optional<ImageSampler::Sample> RandomImageSampler::NextSample(
    bool sample_lens, RandomBitstream& rng) {
  if (sample_index_ == samples_per_pixel_) {
    return std::nullopt;
  }

  double x = pixel_x_ + rng.NextGeometric();
  double y = pixel_y_ + rng.NextGeometric();
  double du = x + subpixel_size_;
  double dv = y + subpixel_size_;

  geometric_t image_u = static_cast<geometric_t>(x / num_pixels_x_);
  geometric_t image_v = static_cast<geometric_t>(y / num_pixels_y_);
  geometric_t image_u_dx = static_cast<geometric_t>(du / num_pixels_x_);
  geometric_t image_v_dv = static_cast<geometric_t>(dv / num_pixels_y_);
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

}  // namespace

std::unique_ptr<ImageSampler> MakeRandomImageSampler(
    uint32_t samples_per_pixel) {
  return std::make_unique<RandomImageSampler>(samples_per_pixel);
}

}  // namespace image_samplers
}  // namespace iris
