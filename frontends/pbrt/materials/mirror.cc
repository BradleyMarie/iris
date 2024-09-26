#include "frontends/pbrt/materials/mirror.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/mirror_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MirrorMaterial;
using ::iris::textures::PointerTexture2D;

constexpr visual kDefaultReflectance = 0.9;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kr", Parameter::REFLECTOR_TEXTURE},
};

class MirrorObjectBuilder : public MaterialBuilder {
 public:
  MirrorObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;
};

class NestedMirrorObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedMirrorObjectBuilder(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<NormalMap> front_normal_map,
      ReferenceCounted<NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        reflectance_(std::move(reflectance)),
        default_(std::make_tuple(
            MakeReferenceCounted<MirrorMaterial>(reflectance_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;

 private:
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  const std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                   ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<NestedMaterialBuilder> MirrorObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);

  auto kr = parameters.find("Kr");
  if (kr != parameters.end()) {
    reflectance_texture = kr->second.GetReflectorTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedMirrorObjectBuilder>(
      std::move(reflectance_texture), std::move(front_normal_map),
      std::move(back_normal_map));
}

MaterialBuilderResult NestedMirrorObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<0>(default_),
                           std::get<1>(default_), std::get<2>(default_));
  }

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture = reflectance_;
  ReferenceCounted<NormalMap> front_normal_map = std::get<1>(default_);
  ReferenceCounted<NormalMap> back_normal_map = std::get<2>(default_);

  auto kr = parameters.find("Kr");
  if (kr != parameters.end()) {
    reflectance_texture = kr->second.GetReflectorTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<Material> material =
      MakeReferenceCounted<MirrorMaterial>(std::move(reflectance_texture));

  return std::make_tuple(material, material, std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_mirror_builder =
    std::make_unique<MirrorObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris