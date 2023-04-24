#include "frontends/pbrt/textures/scale.h"

#include <string_view>

#include "iris/textures/scaled_texture.h"

namespace iris::pbrt_frontend::textures {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_float_parameters = {{"tex1", Parameter::FLOAT_TEXTURE},
                          {"tex2", Parameter::FLOAT_TEXTURE}};

class ScaleFloatTextureBuilder final
    : public ObjectBuilder<void, TextureManager&, const std::string&> {
 public:
  ScaleFloatTextureBuilder() : ObjectBuilder(g_float_parameters) {}

  void Build(const std::unordered_map<std::string_view, Parameter>& parameters,
             TextureManager& texture_manager,
             const std::string& name) const override;
};

void ScaleFloatTextureBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager, const std::string& name) const {
  auto tex1 = texture_manager.AllocateUniformFloatTexture(1.0);
  auto tex2 = texture_manager.AllocateUniformFloatTexture(1.0);

  auto tex1_iter = parameters.find("value");
  if (tex1_iter != parameters.end()) {
    tex1 = tex1_iter->second.GetFloatTextures(1).front();
  }

  auto tex2_iter = parameters.find("value");
  if (tex2_iter != parameters.end()) {
    tex2 = tex1_iter->second.GetFloatTextures(1).front();
  }

  auto tex = MakeReferenceCounted<iris::textures::ScaledValueTexture2D<visual>>(
      std::move(tex1), std::move(tex2));

  texture_manager.Put(name, std::move(tex));
}

}  // namespace

const std::unique_ptr<
    const ObjectBuilder<void, TextureManager&, const std::string&>>
    g_float_scale_builder = std::make_unique<ScaleFloatTextureBuilder>();

}  // namespace iris::pbrt_frontend::textures