#include "iris/image_samplers/internal/halton_sequence.h"

namespace iris {
namespace image_samplers {
namespace internal {

const static int64_t kMaxImageDimensionSize = 59049u;

HaltonSequence::HaltonSequence()
    : sampler_(std::make_unique<Halton_sampler>()), image_dimensions_(0, 0) {
  sampler_->init_faure();
}

bool HaltonSequence::Start(std::pair<size_t, size_t> image_dimensions,
                           std::pair<size_t, size_t> pixel,
                           unsigned sample_index) {
  assert(image_dimensions.second <= kMaxImageDimensionSize);
  assert(image_dimensions.second <= kMaxImageDimensionSize);
  assert(pixel.first < image_dimensions.first);
  assert(pixel.second < image_dimensions.second);

  if (!enumerator_ || image_dimensions != image_dimensions_) {
    enumerator_.emplace(static_cast<unsigned>(image_dimensions.first),
                        static_cast<unsigned>(image_dimensions.second));
  }

  if (sample_index >= enumerator_->get_max_samples_per_pixel()) {
    enumerator_.reset();
    return false;
  }

  sample_index_ =
      enumerator_->get_index(sample_index, static_cast<unsigned>(pixel.first),
                             static_cast<unsigned>(pixel.second));

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
    sample = enumerator_->scale_x(sample) /
             static_cast<geometric_t>(image_dimensions_.first);
  } else if (dimension_ == 1) {
    sample = enumerator_->scale_y(sample) /
             static_cast<geometric_t>(image_dimensions_.second);
  }

  dimension_ += 1;

  return sample;
}

visual_t HaltonSequence::SampleWeight(uint32_t desired_num_samples) const {
  auto max_num_samples = enumerator_->get_max_samples_per_pixel();
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