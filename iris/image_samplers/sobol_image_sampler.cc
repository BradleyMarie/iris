#include "iris/image_samplers/sobol_image_sampler.h"

#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"
#include "iris/image_samplers/internal/sobol_sequence.h"

namespace iris {
namespace image_samplers {

std::unique_ptr<ImageSampler> MakeSobolImageSampler(
    uint32_t desired_samples_per_pixel) {
  return std::make_unique<internal::LowDiscrepancyImageSampler>(
      std::make_unique<internal::SobolSequence>(), desired_samples_per_pixel);
}

}  // namespace image_samplers
}  // namespace iris