#ifndef _FRONTENDS_PBRT_SAMPLERS_RANDOM_
#define _FRONTENDS_PBRT_SAMPLERS_RANDOM_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

std::unique_ptr<ImageSampler> MakeRandom(
    const pbrt_proto::v3::Sampler::Random& random);

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_RANDOM_
