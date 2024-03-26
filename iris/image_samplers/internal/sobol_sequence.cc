#include "iris/image_samplers/internal/sobol_sequence.h"

#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <limits>

#include "third_party/gruenschloss/double/sobol.h"
#include "third_party/gruenschloss/single/sobol.h"
#include "third_party/pbrt/sobolmatrices.h"

namespace iris {
namespace image_samplers {
namespace internal {
namespace {

static const geometric_t kMaxValue = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(0.0));

}  // namespace

std::optional<uint64_t> BitMatrixVectorMultiply(
    const uint64_t bit_matrix[pbrt::SobolMatrixSize], uint64_t bit_vector) {
  if (bit_vector >> static_cast<uint64_t>(pbrt::SobolMatrixSize)) {
    return std::nullopt;
  }

  uint64_t product = 0;
  for (size_t matrix_column = 0; bit_vector;
       bit_vector >>= 1, matrix_column += 1) {
    if (bit_vector & 1) {
      product ^= bit_matrix[matrix_column];
    }
  }

  return product;
}

std::optional<size_t> SobolSequenceIndex(uint64_t log2_resolution,
                                         uint64_t first_dimension,
                                         uint64_t second_dimension,
                                         uint64_t sample_index) {
  if (log2_resolution > 26) {
    return std::nullopt;
  }

  if (log2_resolution == 0) {
    return sample_index;
  }

  size_t matrix_index = log2_resolution - 1;
  auto transformed_sample_index =
      BitMatrixVectorMultiply(pbrt::VdCSobolMatrices[matrix_index],
                              static_cast<uint64_t>(sample_index));
  if (!transformed_sample_index) {
    return std::nullopt;
  }

  uint64_t pixel_index =
      (first_dimension << log2_resolution) | second_dimension;
  uint64_t image_sample_index = pixel_index ^ *transformed_sample_index;

  auto transformed_image_sample_index = BitMatrixVectorMultiply(
      pbrt::VdCSobolMatricesInv[matrix_index], image_sample_index);
  if (!transformed_image_sample_index) {
    return std::nullopt;
  }

  uint64_t num_pixels_log2 = log2_resolution << 1;
  uint64_t sample_index_base = sample_index << num_pixels_log2;

  return sample_index_base ^ *transformed_image_sample_index;
}

bool SobolSequence::Start(std::pair<size_t, size_t> image_dimensions,
                          std::pair<size_t, size_t> pixel,
                          unsigned sample_index) {
  assert(pixel.first < image_dimensions.first);
  assert(pixel.second < image_dimensions.second);

  size_t longest_dimension =
      std::max(image_dimensions.first, image_dimensions.second);
  size_t logical_resolution = std::bit_ceil(longest_dimension);
  size_t logical_resolution_log2 = std::countr_zero(logical_resolution);

  auto maybe_sample_index = SobolSequenceIndex(
      static_cast<uint64_t>(logical_resolution_log2),
      static_cast<uint64_t>(pixel.second), static_cast<uint64_t>(pixel.first),
      static_cast<uint64_t>(sample_index));
  if (!maybe_sample_index) {
    return false;
  }

  to_first_dimension_ = static_cast<geometric_t>(logical_resolution) /
                        static_cast<geometric_t>(image_dimensions.second);
  to_second_dimension_ = static_cast<geometric_t>(logical_resolution) /
                         static_cast<geometric_t>(image_dimensions.first);
  sample_index_ = *maybe_sample_index;
  dimension_ = 0;

  if constexpr (std::is_same<geometric_t, float>::value) {
    num_dimensions_ = sobol32::Matrices::num_dimensions;
  } else {
    num_dimensions_ = sobol64::Matrices::num_dimensions;
  }

  return true;
}

std::optional<geometric_t> SobolSequence::Next() {
  if (dimension_ >= num_dimensions_) {
    return std::nullopt;
  }

  geometric_t sample;
  if constexpr (std::is_same<geometric_t, float>::value) {
    sample = sobol32::sample(sample_index_, dimension_);
  } else {
    sample = sobol64::sample(sample_index_, dimension_);
  }

  if (dimension_ < 2) {
    if (dimension_ == 0) {
      sample *= to_first_dimension_;
    } else {
      sample *= to_second_dimension_;
    }
  }

  dimension_ += 1;

  return std::min(kMaxValue, sample);
}

visual_t SobolSequence::SampleWeight(uint32_t desired_num_samples) const {
  return static_cast<visual_t>(1.0) /
         static_cast<visual_t>(desired_num_samples);
}

void SobolSequence::Discard(size_t num_to_discard) {
  auto samples_remaining = num_dimensions_ - dimension_;

  if (num_to_discard >= samples_remaining) {
    dimension_ = num_dimensions_;
  } else {
    dimension_ += static_cast<unsigned>(num_to_discard);
  }
}

std::unique_ptr<LowDiscrepancySequence> SobolSequence::Duplicate() {
  return std::make_unique<SobolSequence>(*this);
}

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris