#ifndef _FRONTENDS_PBRT_SAMPLERS_RESULT_
#define _FRONTENDS_PBRT_SAMPLERS_RESULT_

#include <functional>
#include <memory>

#include "iris/image_sampler.h"

namespace iris::pbrt_frontend::samplers {

struct Result {
  std::unique_ptr<iris::ImageSampler> image_sampler;
  std::function<void(std::pair<size_t, size_t>)> check_fully_sampled;
};

}  // namespace iris::pbrt_frontend::samplers

#endif  // _FRONTENDS_PBRT_SAMPLERS_RESULT_