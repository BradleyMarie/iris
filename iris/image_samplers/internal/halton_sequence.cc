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
                           uint32_t sample_index) {
  if (image_dimensions.first > std::numeric_limits<unsigned>::max() ||
      image_dimensions.second > std::numeric_limits<unsigned>::max() ||
      pixel.first > std::numeric_limits<unsigned>::max() ||
      pixel.second > std::numeric_limits<unsigned>::max() ||
      sample_index > std::numeric_limits<unsigned>::max()) {
    return false;
  }

  if (!enumerator_ || image_dimensions != image_dimensions_) {
    enumerator_.emplace(static_cast<unsigned>(pixel_.first),
                        static_cast<unsigned>(pixel_.second));
  }

  if (enumerator_->get_max_samples_per_pixel()) {
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