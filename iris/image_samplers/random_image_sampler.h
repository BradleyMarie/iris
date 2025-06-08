#ifndef _IRIS_IMAGE_SAMPLERS_RANDOM_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_RANDOM_IMAGE_SAMPLER_

#include <cstdint>
#include <memory>

#include "iris/image_sampler.h"

namespace iris {
namespace image_samplers {

std::unique_ptr<ImageSampler> MakeRandomImageSampler(
    uint32_t samples_per_pixel);

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_RANDOM_IMAGE_SAMPLER_
