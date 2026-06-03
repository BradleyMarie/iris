#include "frontends/pbrt/samplers/maxmindist.h"

#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/sobol_image_sampler.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::MakeSobolImageSampler;
using ::iris::image_samplers::SobolScrambler;
using ::pbrt_proto::MaxMinDistSampler;

std::unique_ptr<ImageSampler> MakeMaxMinDist(
    const MaxMinDistSampler& maxmindist) {
  return MakeSobolImageSampler(maxmindist.pixelsamples(),
                               SobolScrambler::FastOwen);
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
