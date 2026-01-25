#include "frontends/pbrt/samplers/maxmindist.h"

#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/sobol_image_sampler.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::MakeSobolImageSampler;
using ::iris::image_samplers::SobolScrambler;
using ::pbrt_proto::v3::Sampler;

std::unique_ptr<ImageSampler> MakeMaxMinDist(
    const Sampler::MaxMinDist& maxmindist) {
  return MakeSobolImageSampler(maxmindist.pixelsamples(),
                               SobolScrambler::FastOwen);
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
