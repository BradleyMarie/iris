#include "frontends/pbrt/samplers/halton.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "frontends/pbrt/defaults.h"
#include "iris/image_sampler.h"
#include "iris/image_samplers/halton_image_sampler.h"
#include "pbrt_proto/pbrt.pb.h"
#include "third_party/gruenschloss/halton/halton_enum.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {

using iris::image_samplers::MakeHaltonImageSampler;
using ::pbrt_proto::HaltonSampler;

constexpr size_t kResolution = 128u;

std::unique_ptr<ImageSampler> MakeHalton(const HaltonSampler& halton) {
  HaltonSampler with_defaults = Defaults().samplers().halton();
  with_defaults.MergeFrom(halton);

  Halton_enum enumerator(static_cast<unsigned>(kResolution),
                         static_cast<unsigned>(kResolution));

  if (static_cast<unsigned>(with_defaults.pixelsamples()) >
      enumerator.get_max_samples_per_pixel()) {
    std::cerr << "ERROR: Halton sampler only supports a maximum of "
              << enumerator.get_max_samples_per_pixel() << " samples per pixel"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return MakeHaltonImageSampler(with_defaults.pixelsamples());
}

}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
