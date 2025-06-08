#ifndef _IRIS_IMAGE_SAMPLERS_STRATIFIED_IMAGE_SAMPLER_
#define _IRIS_IMAGE_SAMPLERS_STRATIFIED_IMAGE_SAMPLER_

#include <cstdint>
#include <memory>

#include "iris/image_sampler.h"

namespace iris {
namespace image_samplers {

std::unique_ptr<ImageSampler> MakeStratifiedImageSampler(uint16_t x_samples,
                                                         uint16_t y_samples,
                                                         bool jittered);

}  // namespace image_samplers
}  // namespace iris

#endif  // _IRIS_IMAGE_SAMPLERS_STRATIFIED_IMAGE_SAMPLER_
