#include "iris/image_samplers/stratified_image_sampler.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_sampler.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {
namespace {

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

StratifiedImageSampler::StratifiedImageSampler(uint16_t x_samples,
                                               uint16_t y_samples,
                                               bool jittered) noexcept
    : x_samples_(x_samples),
      y_samples_(y_samples),
      jittered_(jittered),
      sample_weight_(std::pow(x_samples * y_samples, -1.0)),
      num_subpixels_x_(0),
      num_subpixels_y_(0),
      subpixel_x_(0),
      subpixel_y_(0),
      sample_index_(0) {}

void StratifiedImageSampler::StartPixel(
    std::pair<size_t, size_t> image_dimensions,
    std::pair<size_t, size_t> pixel) {
  num_subpixels_x_ = image_dimensions.second * x_samples_;
  num_subpixels_y_ = image_dimensions.first * y_samples_;
  subpixel_x_ = pixel.second * x_samples_;
  subpixel_y_ = pixel.first * y_samples_;
  sample_index_ = 0;
}

std::optional<ImageSampler::Sample> StratifiedImageSampler::NextSample(
    bool sample_lens, Random& rng) {
  if (sample_index_ == x_samples_ * y_samples_) {
    return std::nullopt;
  }

  geometric_t x_offset = 0.5;
  geometric_t y_offset = 0.5;
  if (jittered_) {
    x_offset = rng.NextGeometric();
    y_offset = rng.NextGeometric();
  }

  uint16_t x_index = sample_index_ % x_samples_;
  uint16_t y_index = sample_index_ / x_samples_;
  sample_index_ += 1;

  uint64_t subpixel_x = subpixel_x_ + x_index;
  uint64_t subpixel_y = subpixel_y_ + y_index;

  double x = static_cast<double>(subpixel_x) + x_offset;
  double y = static_cast<double>(subpixel_y) + y_offset;
  double du = static_cast<double>(subpixel_x + 1) + x_offset;
  double dv = static_cast<double>(subpixel_y + 1) + y_offset;

  geometric_t image_u = static_cast<geometric_t>(x / num_subpixels_x_);
  geometric_t image_v = static_cast<geometric_t>(y / num_subpixels_y_);
  geometric_t image_u_dx = static_cast<geometric_t>(du / num_subpixels_x_);
  geometric_t image_v_dv = static_cast<geometric_t>(dv / num_subpixels_y_);

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

std::unique_ptr<ImageSampler> StratifiedImageSampler::Replicate() const {
  return std::make_unique<StratifiedImageSampler>(x_samples_, y_samples_,
                                                  jittered_);
}

}  // namespace

std::unique_ptr<ImageSampler> MakeStratifiedImageSampler(uint16_t x_samples,
                                                         uint16_t y_samples,
                                                         bool jittered) {
  return std::make_unique<StratifiedImageSampler>(x_samples, y_samples,
                                                  jittered);
}

}  // namespace image_samplers
}  // namespace iris
