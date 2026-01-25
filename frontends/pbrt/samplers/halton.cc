#include "frontends/pbrt/samplers/halton.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "iris/image_sampler.h"
#include "iris/image_samplers/halton_image_sampler.h"
#include "pbrt_proto/v3/pbrt.pb.h"
#include "third_party/gruenschloss/halton/halton_enum.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using iris::image_samplers::MakeHaltonImageSampler;
using ::pbrt_proto::v3::Sampler;

constexpr size_t kResolution = 128u;

std::unique_ptr<ImageSampler> MakeHalton(const Sampler::Halton& halton) {
  Halton_enum enumerator(static_cast<unsigned>(kResolution),
                         static_cast<unsigned>(kResolution));

  if (static_cast<unsigned>(halton.pixelsamples()) >
      enumerator.get_max_samples_per_pixel()) {
    std::cerr << "ERROR: Halton sampler only supports a maximum of "
              << enumerator.get_max_samples_per_pixel() << " samples per pixel"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return MakeHaltonImageSampler(halton.pixelsamples());
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
