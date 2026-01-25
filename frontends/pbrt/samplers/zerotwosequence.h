#ifndef _FRONTENDS_PBRT_SAMPLERS_ZEROTWOSEQUENCE_
#define _FRONTENDS_PBRT_SAMPLERS_ZEROTWOSEQUENCE_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

std::unique_ptr<ImageSampler> MakeZeroTwoSequence(
    const pbrt_proto::v3::Sampler::ZeroTwoSequence& zerotwosequence);

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_ZEROTWOSEQUENCE_
