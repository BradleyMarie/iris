#include "iris/image_samplers/internal/sobol_sequence.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_samplers/internal/low_discrepancy_sequence.h"
#include "iris/random_bitstream.h"
#include "third_party/gruenschloss/double/sobol.h"
#include "third_party/gruenschloss/single/sobol.h"
#include "third_party/pbrt/sobolmatrices.h"

namespace iris {
namespace image_samplers {
namespace internal {
namespace {

static const geometric_t kMaxValue = std::nextafter(
    static_cast<geometric_t>(1.0), static_cast<geometric_t>(0.0));

template <typename Result, typename BitVector>
std::optional<Result> BitMatrixVectorMultiply(const Result bit_matrix[],
                                              size_t bit_matrix_size,
                                              BitVector bit_vector) {
  if (static_cast<unsigned>(std::bit_width(bit_vector)) >= bit_matrix_size) {
    return std::nullopt;
  }

  Result product = 0u;
  while (bit_vector) {
    product ^= bit_matrix[std::countr_zero(bit_vector)];
    bit_vector &= bit_vector - 1;
  }

  return product;
}

std::optional<uint64_t> SobolSequenceIndex(uint64_t logical_resolution,
                                           uint64_t first_dimension,
                                           uint64_t second_dimension,
                                           uint64_t sample_index) {
  unsigned log2_resolution = std::countr_zero(logical_resolution);

  if (log2_resolution > 26) {
    return std::nullopt;
  }

  if (log2_resolution == 0) {
    return sample_index;
  }

  std::optional<uint64_t> transformed_sample_index =
      BitMatrixVectorMultiply(pbrt::VdCSobolMatrices[log2_resolution - 1],
                              pbrt::SobolMatrixSize, sample_index);
  if (!transformed_sample_index) {
    return std::nullopt;
  }

  uint64_t pixel_index =
      (first_dimension << log2_resolution) | second_dimension;
  uint64_t image_sample_index = pixel_index ^ *transformed_sample_index;

  std::optional<uint64_t> transformed_image_sample_index =
      BitMatrixVectorMultiply(pbrt::VdCSobolMatricesInv[log2_resolution - 1],
                              pbrt::SobolMatrixSize, image_sample_index);
  if (!transformed_image_sample_index) {
    return std::nullopt;
  }

  uint64_t num_pixels_log2 = log2_resolution << 1;
  uint64_t sample_index_base = sample_index << num_pixels_log2;
  return sample_index_base ^ *transformed_image_sample_index;
}

}  // namespace

void SobolSequence::Permute(RandomBitstream& rng) {
  switch (scrambler_) {
    case Scrambler::None:
      break;
    case Scrambler::FastOwen:
      seed_[0] = rng.Next();
      seed_[1] = rng.Next();
      break;
  }
}

bool SobolSequence::Start(std::pair<size_t, size_t> image_dimensions,
                          std::pair<size_t, size_t> pixel,
                          unsigned sample_index) {
  size_t longest_dimension =
      std::max(image_dimensions.first, image_dimensions.second);
  size_t logical_resolution = std::bit_ceil(longest_dimension);

  std::optional<uint64_t> maybe_sample_index = SobolSequenceIndex(
      logical_resolution, pixel.second, pixel.first, sample_index);
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
  geometric_t sample;
  if constexpr (std::is_same<geometric_t, float>::value) {
    std::optional<uint32_t> value = BitMatrixVectorMultiply(
        sobol32::Matrices::matrices + dimension_ * sobol32::Matrices::size,
        sobol32::Matrices::size, sample_index_);
    if (!value) {
      return std::nullopt;
    }

    if (dimension_ >= 2) {
      switch (scrambler_) {
        case Scrambler::None:
          break;
        case Scrambler::FastOwen:
          *value ^= *value * 0x3D20ADEAu;
          *value += seed_[0];
          *value *= seed_[1] | 1u;
          *value ^= *value * 0x05526C56u;
          *value ^= *value * 0x53A22864u;
          break;
      }
    }

    sample = static_cast<float>(*value) / 4294967296.0f;
  } else {
    if (dimension_ >= num_dimensions_) {
      return std::nullopt;
    }

    sample = sobol64::sample(sample_index_, dimension_);
  }

  if (dimension_ == 0) {
    sample *= to_first_dimension_;
  } else if (dimension_ == 1) {
    sample *= to_second_dimension_;
  }

  dimension_ += 1;

  return std::min(kMaxValue, sample);
}

visual_t SobolSequence::SampleWeight(uint32_t desired_num_samples) const {
  return static_cast<visual_t>(1.0) /
         static_cast<visual_t>(desired_num_samples);
}

void SobolSequence::Discard(size_t num_to_discard) {
  if (num_to_discard >= num_dimensions_ - dimension_) {
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
