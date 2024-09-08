#include "frontends/pbrt/materials/matte.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/matte_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MatteMaterial;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

constexpr visual kDefaultReflectance = 0.5;
constexpr visual kDefaultSigma = 0.0;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kd", Parameter::REFLECTOR_TEXTURE},
        {"sigma", Parameter::FLOAT_TEXTURE},
};

class MatteObjectBuilder : public MaterialBuilder {
 public:
  MatteObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager,
      TextureManager& texture_manager) const override;
};

class NestedMatteObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedMatteObjectBuilder(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
      ReferenceCounted<ValueTexture2D<visual>> sigma,
      ReferenceCounted<NormalMap> front_normal_map,
      ReferenceCounted<NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        diffuse_(std::move(diffuse)),
        sigma_(std::move(sigma)),
        default_(std::make_tuple(
            MakeReferenceCounted<MatteMaterial>(diffuse_, sigma_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager,
      TextureManager& texture_manager) const override;

 private:
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  const ReferenceCounted<ValueTexture2D<visual>> sigma_;
  const std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                   ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<NestedMaterialBuilder> MatteObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  ReferenceCounted<ValueTexture2D<visual>> sigma_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultSigma);

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto sigma = parameters.find("sigma");
  if (sigma != parameters.end()) {
    sigma_texture = sigma->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedMatteObjectBuilder>(
      std::move(diffuse_texture), std::move(sigma_texture),
      std::move(front_normal_map), std::move(back_normal_map));
}

MaterialBuilderResult NestedMatteObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<0>(default_),
                           std::get<1>(default_), std::get<2>(default_));
  }

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture = diffuse_;
  ReferenceCounted<ValueTexture2D<visual>> sigma_texture = sigma_;
  ReferenceCounted<NormalMap> front_normal_map = std::get<1>(default_);
  ReferenceCounted<NormalMap> back_normal_map = std::get<2>(default_);

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto sigma = parameters.find("sigma");
  if (sigma != parameters.end()) {
    sigma_texture = sigma->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<Material> material = MakeReferenceCounted<MatteMaterial>(
      std::move(diffuse_texture), std::move(sigma_texture));

  return std::make_tuple(material, material, std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_matte_builder =
    std::make_unique<MatteObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris