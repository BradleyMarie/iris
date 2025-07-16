#ifndef _FRONTENDS_PBRT_SAMPLERS_MAXMINDIST_
#define _FRONTENDS_PBRT_SAMPLERS_MAXMINDIST_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

std::unique_ptr<ImageSampler> MakeMaxMinDist(
    const pbrt_proto::v3::Sampler::MaxMinDist& maxmindist);

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_MAXMINDIST_
