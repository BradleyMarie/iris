#ifndef _IRIS_IMAGE_SAMPLERS_HALTON_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_HALTON_IMAGE_SAMPLER_

#include <cstdint>
#include <memory>

#include "iris/image_sampler.h"

namespace iris {
namespace image_samplers {

std::unique_ptr<ImageSampler> MakeHaltonImageSampler(
    uint32_t desired_samples_per_pixel);

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_HALTON_IMAGE_SAMPLER_
