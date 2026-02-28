#ifndef _FRONTENDS_PBRT_SAMPLERS_HALTON_
#define _FRONTENDS_PBRT_SAMPLERS_HALTON_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

std::unique_ptr<ImageSampler> MakeHalton(
    const pbrt_proto::HaltonSampler& halton);

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_HALTON_
