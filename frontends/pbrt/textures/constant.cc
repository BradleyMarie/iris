#include "frontends/pbrt/textures/constant.h"

#include <string_view>

#include "iris/textures/constant_texture.h"

namespace iris::pbrt_frontend::textures {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_float_parameters = {{"value", Parameter::FLOAT}};

class ConstantFloatTextureBuilder final
    : public ObjectBuilder<void, ImageManager&, TextureManager&,
                           const std::string&> {
 public:
  ConstantFloatTextureBuilder() : ObjectBuilder(g_float_parameters) {}

  void Build(const std::unordered_map<std::string_view, Parameter>& parameters,
             ImageManager& image_manager, TextureManager& texture_manager,
             const std::string& name) const override;
};

void ConstantFloatTextureBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    ImageManager& image_manager, TextureManager& texture_manager,
    const std::string& name) const {
  geometric value = 1.0;

  auto value_iter = parameters.find("value");
  if (value_iter != parameters.end()) {
    value = value_iter->second.GetFloatValues(1).front();
    if (!std::isfinite(value)) {
      std::cerr << "ERROR: Out of range value for parameter: value"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto result = texture_manager.AllocateUniformFloatTexture(value);

  texture_manager.Put(name, std::move(result));
}

static const std::unordered_map<std::string_view, Parameter::Type>
    g_spectrum_parameters = {{"value", Parameter::REFLECTOR}};

class ConstantSpectrumTextureBuilder final
    : public ObjectBuilder<void, ImageManager&, TextureManager&,
                           const std::string&> {
 public:
  ConstantSpectrumTextureBuilder() : ObjectBuilder(g_spectrum_parameters) {}

  void Build(const std::unordered_map<std::string_view, Parameter>& parameters,
             ImageManager& image_manager, TextureManager& texture_manager,
             const std::string& name) const override;
};

void ConstantSpectrumTextureBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    ImageManager& image_manager, TextureManager& texture_manager,
    const std::string& name) const {
  ReferenceCounted<
      iris::textures::PointerTexture2D<Reflector, SpectralAllocator>>
      texture;

  auto value = parameters.find("value");
  if (value != parameters.end()) {
    auto reflector = value->second.GetReflectors(1).front();
    texture =
        texture_manager.AllocateUniformReflectorTexture(std::move(reflector));
  } else {
    texture = texture_manager.AllocateUniformReflectorTexture(
        static_cast<visual>(1.0));
  }

  texture_manager.Put(name, std::move(texture));
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<void, ImageManager&, TextureManager&,
                                          const std::string&>>
    g_float_constant_builder = std::make_unique<ConstantFloatTextureBuilder>();

const std::unique_ptr<const ObjectBuilder<void, ImageManager&, TextureManager&,
                                          const std::string&>>
    g_spectrum_constant_builder =
        std::make_unique<ConstantSpectrumTextureBuilder>();

}  // namespace iris::pbrt_frontend::textures