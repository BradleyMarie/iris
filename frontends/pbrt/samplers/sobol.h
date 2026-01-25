#ifndef _FRONTENDS_PBRT_SAMPLERS_SOBOL_
#define _FRONTENDS_PBRT_SAMPLERS_SOBOL_

#include <memory>

#include "iris/image_sampler.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

std::unique_ptr<ImageSampler> MakeSobol(
    const pbrt_proto::v3::Sampler::Sobol& sobol);

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SAMPLERS_SOBOL_
