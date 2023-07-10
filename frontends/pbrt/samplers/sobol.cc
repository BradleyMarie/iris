#include "frontends/pbrt/samplers/sobol.h"

#include <limits>

#include "iris/image_samplers/sobol_image_sampler.h"

namespace iris::pbrt_frontend::samplers {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"pixelsamples", Parameter::INTEGER},
};

class SobolObjectBuilder
    : public ObjectBuilder<std::unique_ptr<iris::ImageSampler>> {
 public:
  SobolObjectBuilder() : ObjectBuilder(g_parameters) {}

  std::unique_ptr<iris::ImageSampler> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters)
      const override;
};

std::unique_ptr<iris::ImageSampler> SobolObjectBuilder::Build(
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

  return iris::image_samplers::MakeSobolImageSampler(pixel_samples);
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<std::unique_ptr<iris::ImageSampler>>>
    g_sobol_builder(std::make_unique<SobolObjectBuilder>());

}  // namespace iris::pbrt_frontend::samplers