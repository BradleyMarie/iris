#include "frontends/pbrt/samplers/random.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/random_image_sampler.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::RandomImageSampler;
using ::pbrt_proto::v3::Sampler;

std::unique_ptr<ImageSampler> MakeRandom(const Sampler::Random& random) {
  if (random.pixelsamples() < 0) {
    std::cerr << "ERROR: Out of range value for parameter: pixelsamples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return std::make_unique<RandomImageSampler>(
      static_cast<uint32_t>(random.pixelsamples()));
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
