#include "frontends/pbrt/samplers/random.h"

#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/random_image_sampler.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::MakeRandomImageSampler;
using ::pbrt_proto::v3::Sampler;

std::unique_ptr<ImageSampler> MakeRandom(const Sampler::Random& random) {
  return MakeRandomImageSampler(random.pixelsamples());
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
