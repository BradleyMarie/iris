#include "frontends/pbrt/samplers/halton.h"

#include <limits>

#include "iris/image_samplers/halton_image_sampler.h"
#include "third_party/gruenschloss/halton/halton_enum.h"

namespace iris::pbrt_frontend::samplers {
namespace {

static const size_t kResolution = 128u;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"pixelsamples", Parameter::INTEGER},
};

class HaltonObjectBuilder
    : public ObjectBuilder<std::unique_ptr<iris::ImageSampler>> {
 public:
  HaltonObjectBuilder() : ObjectBuilder(g_parameters) {}

  std::unique_ptr<iris::ImageSampler> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters)
      const override;
};

std::unique_ptr<iris::ImageSampler> HaltonObjectBuilder::Build(
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

    Halton_enum enumerator(static_cast<unsigned>(kResolution),
                           static_cast<unsigned>(kResolution));

    if (value > enumerator.get_max_samples_per_pixel()) {
      std::cerr << "ERROR: Halton sampler only supports a maximum of "
                << enumerator.get_max_samples_per_pixel()
                << " samples per pixel" << std::endl;
      exit(EXIT_FAILURE);
    }

    pixel_samples = value;
  }

  return iris::image_samplers::MakeHaltonImageSampler(pixel_samples);
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<std::unique_ptr<iris::ImageSampler>>>
    g_halton_builder(std::make_unique<HaltonObjectBuilder>());

}  // namespace iris::pbrt_frontend::samplers