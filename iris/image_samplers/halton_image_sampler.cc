#include "iris/image_samplers/halton_image_sampler.h"

#include <cstdint>
#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/internal/halton_sequence.h"
#include "iris/image_samplers/internal/low_discrepancy_image_sampler.h"

namespace iris {
namespace image_samplers {

using ::iris::image_samplers::internal::HaltonSequence;
using ::iris::image_samplers::internal::LowDiscrepancyImageSampler;

std::unique_ptr<ImageSampler> MakeHaltonImageSampler(
    uint32_t desired_samples_per_pixel) {
  return std::make_unique<LowDiscrepancyImageSampler>(
      std::make_unique<HaltonSequence>(), desired_samples_per_pixel);
}

}  // namespace image_samplers
}  // namespace iris
