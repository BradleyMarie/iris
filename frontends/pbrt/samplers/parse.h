#ifndef _FRONTENDS_PBRT_SAMPLERS_PARSE_
#define _FRONTENDS_PBRT_SAMPLERS_PARSE_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

std::unique_ptr<ImageSampler> ParseSamper(
    const pbrt_proto::v3::Sampler& sampler);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_PARSE_
