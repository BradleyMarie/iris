#include "frontends/pbrt/samplers/sobol.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/sobol_image_sampler.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using ::iris::image_samplers::MakeSobolImageSampler;
using ::pbrt_proto::v3::Sampler;

std::unique_ptr<ImageSampler> MakeSobol(const Sampler::Sobol& sobol) {
  if (sobol.pixelsamples() < 0) {
    std::cerr << "ERROR: Out of range value for parameter: pixelsamples"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return MakeSobolImageSampler(static_cast<uint32_t>(sobol.pixelsamples()));
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
