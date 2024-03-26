#include "iris/image_samplers/internal/halton_sequence.h"

#include <algorithm>
#include <cmath>

namespace iris {
namespace image_samplers {
namespace internal {
namespace {

static const size_t kResolution = 128u;
static const Halton_enum kHaltonEnumerator(kResolution, kResolution);
static const geometric_t kMaxValue = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(0.0));

}  // namespace

HaltonSequence::HaltonSequence()
    : sampler_(std::make_unique<Halton_sampler>()), image_dimensions_(0, 0) {
  sampler_->init_faure();
}

bool HaltonSequence::Start(std::pair<size_t, size_t> image_dimensions,
                           std::pair<size_t, size_t> pixel,
                           unsigned sample_index) {
  assert(pixel.first < image_dimensions.first);
  assert(pixel.second < image_dimensions.second);

  if (sample_index >= kHaltonEnumerator.get_max_samples_per_pixel()) {
    return false;
  }

  x_offset_ = (pixel.second / kResolution) * kResolution;
  y_offset_ = (pixel.first / kResolution) * kResolution;

  sample_index_ = kHaltonEnumerator.get_index(
      sample_index, static_cast<unsigned>(pixel.second % kResolution),
      static_cast<unsigned>(pixel.first % kResolution));

  image_dimensions_ = image_dimensions;
  dimension_ = 0;

  return true;
}

std::optional<geometric_t> HaltonSequence::Next() {
  if (dimension_ >= sampler_->get_num_dimensions()) {
    return std::nullopt;
  }

  geometric_t sample = sampler_->sample(dimension_, sample_index_);

  if (dimension_ == 0) {
    sample = (kHaltonEnumerator.scale_x(sample) + x_offset_) /
             static_cast<geometric_t>(image_dimensions_.second);
  } else if (dimension_ == 1) {
    sample = (kHaltonEnumerator.scale_y(sample) + y_offset_) /
             static_cast<geometric_t>(image_dimensions_.first);
  }

  dimension_ += 1;

  return std::min(kMaxValue, sample);
}

visual_t HaltonSequence::SampleWeight(uint32_t desired_num_samples) const {
  auto max_num_samples = kHaltonEnumerator.get_max_samples_per_pixel();
  auto num_samples = desired_num_samples < max_num_samples ? desired_num_samples
                                                           : max_num_samples;
  return static_cast<visual_t>(1.0) / static_cast<visual_t>(num_samples);
}

void HaltonSequence::Discard(size_t num_to_discard) {
  auto dimensions_remaining = sampler_->get_num_dimensions() - dimension_;

  if (num_to_discard >= dimensions_remaining) {
    dimension_ = sampler_->get_num_dimensions();
  } else {
    dimension_ += static_cast<unsigned>(num_to_discard);
  }
}

std::unique_ptr<LowDiscrepancySequence> HaltonSequence::Duplicate() {
  return std::make_unique<HaltonSequence>(*this);
}

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris