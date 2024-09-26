#include "frontends/pbrt/materials/substrate.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/substrate_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::SubstrateMaterial;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

constexpr visual kDefaultReflectance = 0.5;
constexpr visual kDefaultRoughness = 0.1;
constexpr bool kDefaultRemapRoughness = true;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kd", Parameter::REFLECTOR_TEXTURE},
        {"Ks", Parameter::REFLECTOR_TEXTURE},
        {"uroughness", Parameter::FLOAT_TEXTURE},
        {"vroughness", Parameter::FLOAT_TEXTURE},
        {"remaproughness", Parameter::BOOL},
};

class SubstrateObjectBuilder : public MaterialBuilder {
 public:
  SubstrateObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;
};

class NestedSubstrateObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedSubstrateObjectBuilder(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
      ReferenceCounted<ValueTexture2D<visual>> u_roughness,
      ReferenceCounted<ValueTexture2D<visual>> v_roughness,
      bool remap_roughness, ReferenceCounted<NormalMap> front_normal_map,
      ReferenceCounted<NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        u_roughness_(std::move(u_roughness)),
        v_roughness_(std::move(v_roughness)),
        remap_roughness_(remap_roughness),
        default_(std::make_tuple(MakeReferenceCounted<SubstrateMaterial>(
                                     diffuse_, specular_, u_roughness_,
                                     v_roughness_, remap_roughness_),
                                 std::move(front_normal_map),
                                 std::move(back_normal_map))) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;

 private:
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  const ReferenceCounted<ValueTexture2D<visual>> u_roughness_;
  const ReferenceCounted<ValueTexture2D<visual>> v_roughness_;
  bool remap_roughness_;
  const std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                   ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<NestedMaterialBuilder> SubstrateObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  ReferenceCounted<ValueTexture2D<visual>> u_roughness_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultRoughness);
  ReferenceCounted<ValueTexture2D<visual>> v_roughness_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultRoughness);
  bool remap_roughness = kDefaultRemapRoughness;

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto ks = parameters.find("Ks");
  if (ks != parameters.end()) {
    specular_texture = ks->second.GetReflectorTextures(1).front();
  }

  auto u_roughness = parameters.find("uroughness");
  if (u_roughness != parameters.end()) {
    u_roughness_texture = u_roughness->second.GetFloatTextures(1).front();
  }

  auto v_roughness = parameters.find("vroughness");
  if (v_roughness != parameters.end()) {
    v_roughness_texture = v_roughness->second.GetFloatTextures(1).front();
  }

  auto remaproughness = parameters.find("remaproughness");
  if (remaproughness != parameters.end()) {
    remap_roughness = remaproughness->second.GetBoolValues(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedSubstrateObjectBuilder>(
      std::move(diffuse_texture), std::move(specular_texture),
      std::move(u_roughness_texture), std::move(v_roughness_texture),
      remap_roughness, std::move(front_normal_map), std::move(back_normal_map));
}

MaterialBuilderResult NestedSubstrateObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<0>(default_),
                           std::get<1>(default_), std::get<2>(default_));
  }

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture = diffuse_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture = specular_;
  ReferenceCounted<ValueTexture2D<visual>> u_roughness_texture = u_roughness_;
  ReferenceCounted<ValueTexture2D<visual>> v_roughness_texture = v_roughness_;
  bool remap_roughness = remap_roughness_;
  ReferenceCounted<NormalMap> front_normal_map = std::get<1>(default_);
  ReferenceCounted<NormalMap> back_normal_map = std::get<2>(default_);

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto ks = parameters.find("Ks");
  if (ks != parameters.end()) {
    specular_texture = ks->second.GetReflectorTextures(1).front();
  }

  auto u_roughness = parameters.find("uroughness");
  if (u_roughness != parameters.end()) {
    u_roughness_texture = u_roughness->second.GetFloatTextures(1).front();
  }

  auto v_roughness = parameters.find("vroughness");
  if (v_roughness != parameters.end()) {
    v_roughness_texture = v_roughness->second.GetFloatTextures(1).front();
  }

  auto remaproughness = parameters.find("remaproughness");
  if (remaproughness != parameters.end()) {
    remap_roughness = remaproughness->second.GetBoolValues(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<Material> material = MakeReferenceCounted<SubstrateMaterial>(
      std::move(diffuse_texture), std::move(specular_texture),
      std::move(u_roughness_texture), std::move(v_roughness_texture),
      remap_roughness);

  return std::make_tuple(material, material, std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_substrate_builder =
    std::make_unique<SubstrateObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris