#include "iris/image_samplers/internal/halton_sequence.h"

namespace iris {
namespace image_samplers {
namespace internal {

HaltonSequence::HaltonSequence()
    : sampler_(std::make_unique<Halton_sampler>()),
      image_dimensions_(0, 0),
      pixel_(0, 0) {
  sampler_->init_faure();
}

bool HaltonSequence::Start(std::pair<size_t, size_t> image_dimensions,
                           std::pair<size_t, size_t> pixel,
                           unsigned sample_index) {
  if (image_dimensions.first > std::numeric_limits<unsigned>::max() ||
      image_dimensions.second > std::numeric_limits<unsigned>::max() ||
      pixel.first > std::numeric_limits<unsigned>::max() ||
      pixel.second > std::numeric_limits<unsigned>::max()) {
    return false;
  }

  if (!enumerator_ || image_dimensions != image_dimensions_) {
    enumerator_.emplace(static_cast<unsigned>(pixel_.first),
                        static_cast<unsigned>(pixel_.second));
  }

  if (enumerator_->get_max_samples_per_pixel() >= sample_index) {
    enumerator_.reset();
    return false;
  }

  sample_index_ = sample_index;
  dimension_ = 0;

  return true;
}

std::optional<geometric_t> HaltonSequence::Next() {
  if (dimension_ > sampler_->get_num_dimensions()) {
    return std::nullopt;
  }

  auto index =
      enumerator_->get_index(sample_index_, static_cast<unsigned>(pixel_.first),
                             static_cast<unsigned>(pixel_.second));

  return sampler_->sample(dimension_++, index);
}

visual_t HaltonSequence::SampleWeight(uint32_t desired_num_samples) const {
  auto max_num_samples = enumerator_->get_max_samples_per_pixel();
  auto num_samples = desired_num_samples < max_num_samples ? desired_num_samples
                                                           : max_num_samples;
  return static_cast<visual_t>(1.0) / static_cast<visual_t>(num_samples);
}

void HaltonSequence::Discard(size_t num_to_discard) {
  assert(num_to_discard < std::numeric_limits<unsigned>::max());
  dimension_ += static_cast<unsigned>(num_to_discard);
}

std::unique_ptr<LowDiscrepancySequence> HaltonSequence::Duplicate() {
  return std::make_unique<HaltonSequence>(*this);
}

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris