#ifndef _IRIS_IMAGE_SAMPLERS_SOBOL_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_SOBOL_IMAGE_SAMPLER_

#include <memory>

#include "iris/image_sampler.h"

namespace iris {
namespace image_samplers {

enum class SobolScrambler {
  None,
  FastOwen,
};

std::unique_ptr<ImageSampler> MakeSobolImageSampler(
    uint32_t desired_samples_per_pixel, SobolScrambler scrambler);

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_SOBOL_IMAGE_SAMPLER_
