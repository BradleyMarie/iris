#include "frontends/pbrt/samplers/halton.h"

#include <limits>

#include "iris/image_samplers/halton_image_sampler.h"
#include "third_party/gruenschloss/halton/halton_enum.h"

namespace iris::pbrt_frontend::samplers {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"pixelsamples", Parameter::INTEGER},
};

class HaltonObjectBuilder : public ObjectBuilder<Result> {
 public:
  HaltonObjectBuilder() : ObjectBuilder(g_parameters) {}

  Result Build(const std::unordered_map<std::string_view, Parameter>&
                   parameters) const override;
};

Result HaltonObjectBuilder::Build(
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

  auto fully_sampled = [pixel_samples](std::pair<size_t, size_t> dimensions) {
    assert(dimensions.first <= std::numeric_limits<unsigned>::max());
    assert(dimensions.second <= std::numeric_limits<unsigned>::max());

    Halton_enum enumerator(static_cast<unsigned>(dimensions.first),
                           static_cast<unsigned>(dimensions.second));

    if (pixel_samples > enumerator.get_max_samples_per_pixel()) {
      std::cerr << "ERROR: At resolution " << dimensions.second << "x"
                << dimensions.first << " Halton sampler only supports "
                << enumerator.get_max_samples_per_pixel()
                << " samples per pixel" << std::endl;
      exit(EXIT_FAILURE);
    }
  };

  return {iris::image_samplers::MakeHaltonImageSampler(pixel_samples),
          fully_sampled};
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<Result>> g_halton_builder(
    std::make_unique<HaltonObjectBuilder>());

}  // namespace iris::pbrt_frontend::samplers