#include "frontends/pbrt/samplers/halton.h"

#include <limits>

#include "iris/image_samplers/halton_image_sampler.h"

namespace iris::pbrt_frontend::samplers {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"pixelsamples", Parameter::INTEGER},
};

class HaltonObjectBuilder
    : public ObjectBuilder<std::unique_ptr<ImageSampler>> {
 public:
  HaltonObjectBuilder() : ObjectBuilder(g_parameters) {}

  std::unique_ptr<ImageSampler> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters)
      const override;
};

std::unique_ptr<ImageSampler> HaltonObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters) const {
  uint32_t pixel_samples = 16;

  auto pixelsamples = parameters.find("pixelsamples");
  if (pixelsamples != parameters.end()) {
    auto value = pixelsamples->second.GetIntegerValues(1).front();
    if (value < 0 || value > std::numeric_limits<uint32_t>::max()) {
      std::cerr << "ERROR: Out of range value for parameter: pixelsamples"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    pixel_samples = value;
  }

  return iris::image_samplers::MakeHaltonImageSampler(pixel_samples);
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<std::unique_ptr<ImageSampler>>>
    g_halton_builder(std::make_unique<HaltonObjectBuilder>());

}  // namespace iris::pbrt_frontend::samplers