#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_sampler.h"
#include "iris/image_samplers/internal/low_discrepancy_sequence.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {
namespace internal {

void LowDiscrepancyImageSampler::StartPixel(
    std::pair<size_t, size_t> image_dimensions, std::pair<size_t, size_t> pixel,
    Random& rng) {
  sequence_->Permute(rng);

  image_dimensions_ = image_dimensions;
  pixel_ = pixel;

  geometric_t pixel_x_size = static_cast<geometric_t>(1.0) /
                             static_cast<geometric_t>(image_dimensions.second);
  geometric_t pixel_y_size = static_cast<geometric_t>(1.0) /
                             static_cast<geometric_t>(image_dimensions.first);

  subpixel_size_x_ = pixel_x_size;
  subpixel_size_y_ = pixel_y_size;

  if (desired_samples_per_pixel_ != 0) {
    geometric_t samples_per_dimension =
        std::sqrt(static_cast<double>(desired_samples_per_pixel_));
    subpixel_size_x_ /= samples_per_dimension;
    subpixel_size_y_ /= samples_per_dimension;
  }

  sample_index_ = 0;
}

std::optional<ImageSampler::Sample> LowDiscrepancyImageSampler::NextSample(
    bool sample_lens, Random& rng) {
  if (!sequence_) {
    return std::nullopt;
  }

  if (sample_index_ == desired_samples_per_pixel_) {
    return std::nullopt;
  }

  if (!sequence_->Start(image_dimensions_, pixel_, sample_index_)) {
    return std::nullopt;
  }

  geometric_t image_u = sequence_->NextGeometric();
  geometric_t image_v = sequence_->NextGeometric();
  geometric_t image_u_dx = image_u + subpixel_size_x_;
  geometric_t image_v_dv = image_v + subpixel_size_y_;
  visual_t sample_weight = sequence_->SampleWeight(desired_samples_per_pixel_);
  sample_index_ += 1;

  std::optional<std::array<geometric_t, 2>> lens_uv;
  if (sample_lens) {
    lens_uv.emplace(std::array<geometric_t, 2>{sequence_->NextGeometric(),
                                               sequence_->NextGeometric()});
  }

  return ImageSampler::Sample{{image_u, image_v},
                              {{image_u_dx, image_v_dv}},
                              lens_uv,
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
