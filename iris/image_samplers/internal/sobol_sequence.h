#ifndef _IRIS_IMAGE_SAMPLERS_INTERNAL_SOBOL_SEQUENCE_
#define _IRIS_IMAGE_SAMPLERS_INTERNAL_SOBOL_SEQUENCE_

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "iris/float.h"
#include "iris/image_samplers/internal/low_discrepancy_sequence.h"
#include "iris/random.h"

namespace iris {
namespace image_samplers {
namespace internal {

class SobolSequence final : public LowDiscrepancySequence {
 public:
  enum class Scrambler {
    None,
    FastOwen,
  };

  SobolSequence(Scrambler scrambler) : scrambler_(scrambler) {}
  SobolSequence(const SobolSequence& to_copy) = default;

  void Permute(Random& random) override;
  bool Start(std::pair<size_t, size_t> image_dimensions,
             std::pair<size_t, size_t> pixel, unsigned sample_index) override;
  std::optional<geometric_t> Next() override;
  void Discard(size_t num_to_discard) override;

  visual_t SampleWeight(uint32_t desired_num_samples) const override;

  std::unique_ptr<LowDiscrepancySequence> Duplicate() override;

 private:
  geometric_t to_first_dimension_;
  geometric_t to_second_dimension_;
  unsigned long long sample_index_;
  unsigned dimension_;
  unsigned num_dimensions_;
  uint64_t seed64_[2];
  uint32_t seed32_[2];
  Scrambler scrambler_;
};

}  // namespace internal
}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_INTERNAL_SOBOL_SEQUENCE_
