#include "iris/image_samplers/internal/sobol_sequence.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
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

constexpr unsigned NumDimensions() {
  if constexpr (std::is_same<geometric_t, float>::value) {
    return sobol32::Matrices::num_dimensions;
  } else {
    return sobol64::Matrices::num_dimensions;
  }
}

template <typename Result, typename BitVector>
Result BitMatrixVectorMultiply(const Result bit_matrix[],
                               BitVector bit_vector) {
  Result product = 0u;
  while (bit_vector) {
    product ^= bit_matrix[std::countr_zero(bit_vector)];
    bit_vector &= bit_vector - 1;
  }

  return product;
}

template <typename Result, typename BitVector>
std::optional<Result> BitMatrixVectorMultiply(const Result bit_matrix[],
                                              size_t bit_matrix_size,
                                              BitVector bit_vector) {
  if (static_cast<unsigned>(std::bit_width(bit_vector)) >= bit_matrix_size) {
    return std::nullopt;
  }

  return BitMatrixVectorMultiply(bit_matrix, bit_vector);
}

}  // namespace

SobolSequence::SobolSequence(Scrambler scrambler) : scrambler_(scrambler) {
  if constexpr (std::is_same<geometric_t, float>::value) {
    to_dimension_[2] = 1.0f / 4294967296.0f;
  } else {
    to_dimension_[2] = 1.0 / 4503599627370496.0;
  }
}

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

  if (longest_dimension == 1) {
    sample_index_ = sample_index;
    to_dimension_[0] = static_cast<geometric_t>(1.0);
    to_dimension_[1] = static_cast<geometric_t>(1.0);
  } else {
    if (std::bit_width(longest_dimension) >
        std::numeric_limits<geometric_t>::digits) {
      return false;
    }

    size_t logical_resolution = std::bit_ceil(longest_dimension);
    size_t log2_resolution = std::countr_zero(logical_resolution);
    size_t num_pixels_log2 = log2_resolution << 1;

    std::optional<uint64_t> transformed_sample_index =
        BitMatrixVectorMultiply(pbrt::VdCSobolMatrices[log2_resolution - 1],
                                pbrt::SobolMatrixSize, sample_index);
    if (!transformed_sample_index) {
      return false;
    }

    uint64_t pixel_index =
        (static_cast<uint64_t>(pixel.second) << log2_resolution) | pixel.first;
    uint64_t image_sample_index = pixel_index ^ *transformed_sample_index;

    std::optional<uint64_t> transformed_image_sample_index =
        BitMatrixVectorMultiply(pbrt::VdCSobolMatricesInv[log2_resolution - 1],
                                pbrt::SobolMatrixSize, image_sample_index);
    if (!transformed_image_sample_index) {
      return false;
    }

    uint64_t next_sample_index =
        *transformed_image_sample_index ^
        (static_cast<uint64_t>(sample_index) << num_pixels_log2);

    if constexpr (std::is_same<geometric_t, float>::value) {
      if (static_cast<unsigned>(std::bit_width(next_sample_index)) >=
          sobol32::Matrices::size) {
        return false;
      }
    } else {
      if (static_cast<unsigned>(std::bit_width(next_sample_index)) >=
          sobol64::Matrices::size) {
        return false;
      }
    }

    sample_index_ = next_sample_index;
    to_dimension_[0] = (static_cast<geometric_t>(logical_resolution) /
                        static_cast<geometric_t>(image_dimensions.second)) *
                       to_dimension_[2];
    to_dimension_[1] = (static_cast<geometric_t>(logical_resolution) /
                        static_cast<geometric_t>(image_dimensions.first)) *
                       to_dimension_[2];
  }

  dimension_ = 0;

  return true;
}

std::optional<geometric_t> SobolSequence::Next() {
  if (dimension_ >= NumDimensions()) {
    return std::nullopt;
  }

  geometric_t sample;
  unsigned dimension_index = dimension_;
  if constexpr (std::is_same<geometric_t, float>::value) {
    uint32_t product = BitMatrixVectorMultiply(
        sobol32::Matrices::matrices + dimension_ * sobol32::Matrices::size,
        sample_index_);

    if (dimension_ >= 2) {
      switch (scrambler_) {
        case Scrambler::None:
          break;
        case Scrambler::FastOwen:
          product ^= product * 0x3D20ADEAu;
          product += seed_[0];
          product *= seed_[1] | 1u;
          product ^= product * 0x05526C56u;
          product ^= product * 0x53A22864u;
          break;
      }

      dimension_index = 2;
    }

    sample = static_cast<float>(product) * to_dimension_[dimension_index];
  } else {
    uint64_t product = BitMatrixVectorMultiply(
        sobol64::Matrices::matrices + dimension_ * sobol64::Matrices::size,
        sample_index_);

    uint32_t top_bits;
    uint64_t result;
    if (dimension_ >= 2) {
      switch (scrambler_) {
        case Scrambler::None:
          result = product;
          break;
        case Scrambler::FastOwen:
          top_bits = product >> 20;
          top_bits ^= top_bits * 0x3D20ADEAu;
          top_bits += seed_[0];
          top_bits *= seed_[1] | 1u;
          top_bits ^= top_bits * 0x05526C56u;
          top_bits ^= top_bits * 0x53A22864u;
          result = top_bits;
          result <<= 20;
          result |= product & 0xFFFFFu;
          break;
      }

      dimension_index = 2;
    }

    sample = static_cast<double>(result) * to_dimension_[dimension_index];
  }

  dimension_ += 1;

  return std::min(kMaxValue, sample);
}

visual_t SobolSequence::SampleWeight(uint32_t desired_num_samples) const {
  return static_cast<visual_t>(1.0) /
         static_cast<visual_t>(desired_num_samples);
}

void SobolSequence::Discard(size_t num_to_discard) {
  if (num_to_discard >= NumDimensions() - dimension_) {
    dimension_ = NumDimensions();
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
