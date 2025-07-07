#include "iris/image_samplers/sobol_image_sampler.h"

#include "iris/image_sampler.h"
#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"
#include "iris/image_samplers/internal/sobol_sequence.h"

namespace iris {
namespace image_samplers {

using ::iris::image_samplers::internal::LowDiscrepancyImageSampler;
using ::iris::image_samplers::internal::SobolSequence;

std::unique_ptr<ImageSampler> MakeSobolImageSampler(
    uint32_t desired_samples_per_pixel, SobolScrambler scrambler) {
  SobolSequence::Scrambler internal_scrambler = SobolSequence::Scrambler::None;
  switch (scrambler) {
    case SobolScrambler::None:
      internal_scrambler = SobolSequence::Scrambler::None;
      break;
    case SobolScrambler::FastOwen:
      internal_scrambler = SobolSequence::Scrambler::FastOwen;
      break;
  }

  return std::make_unique<LowDiscrepancyImageSampler>(
      std::make_unique<SobolSequence>(internal_scrambler),
      desired_samples_per_pixel);
}

}  // namespace image_samplers
}  // namespace iris
