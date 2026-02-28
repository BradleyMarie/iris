#ifndef _FRONTENDS_PBRT_SAMPLERS_ZEROTWOSEQUENCE_
#define _FRONTENDS_PBRT_SAMPLERS_ZEROTWOSEQUENCE_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

std::unique_ptr<ImageSampler> MakeZeroTwoSequence(
    const pbrt_proto::ZeroTwoSequenceSampler& zerotwosequence);

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_ZEROTWOSEQUENCE_
