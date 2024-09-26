#include "frontends/pbrt/materials/uber.h"

#include <algorithm>
#include <utility>

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/uber_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::UberMaterial;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

constexpr visual kDefaultDiffuse = 0.25;
constexpr visual kDefaultEtaFront = 1.0;
constexpr visual kDefaultEtaBack = 1.5;
constexpr visual kDefaultOpacity = 1.0;
constexpr visual kDefaultSpecular = 0.25;
constexpr visual kDefaultRoughness = 0.1;
constexpr visual kDefaultReflectance = 0.0;
constexpr visual kDefaultTransmittance = 0.0;
constexpr bool kDefaultRemapRoughness = true;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kd", Parameter::REFLECTOR_TEXTURE},
        {"Ks", Parameter::REFLECTOR_TEXTURE},
        {"Kr", Parameter::REFLECTOR_TEXTURE},
        {"Kt", Parameter::REFLECTOR_TEXTURE},
        {"eta", Parameter::FLOAT_TEXTURE},
        {"opacity", Parameter::FLOAT_TEXTURE},
        {"roughness", Parameter::FLOAT_TEXTURE},
        {"uroughness", Parameter::FLOAT_TEXTURE},
        {"vroughness", Parameter::FLOAT_TEXTURE},
        {"remaproughness", Parameter::BOOL},
};

class UberObjectBuilder : public MaterialBuilder {
 public:
  UberObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;
};

class NestedUberObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedUberObjectBuilder(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
      ReferenceCounted<ValueTexture2D<visual>> opacity,
      ReferenceCounted<ValueTexture2D<visual>> eta_front,
      ReferenceCounted<ValueTexture2D<visual>> eta_back,
      ReferenceCounted<ValueTexture2D<visual>> roughness,
      ReferenceCounted<ValueTexture2D<visual>> uroughness,
      ReferenceCounted<ValueTexture2D<visual>> vroughness, bool remap_roughness,
      ReferenceCounted<NormalMap> front_bump,
      ReferenceCounted<NormalMap> back_bump)
      : ObjectBuilder(g_parameters),
        reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        opacity_(std::move(opacity)),
        eta_front_(std::move(eta_front)),
        eta_back_(std::move(eta_back)),
        roughness_(std::move(roughness)),
        uroughness_(std::move(uroughness)),
        vroughness_(std::move(vroughness)),
        remap_roughness_(remap_roughness),
        default_(std::make_tuple(
            MakeReferenceCounted<materials::UberMaterial>(
                reflectance_, transmittance_, diffuse_, specular_, opacity_,
                eta_front_, eta_back_, uroughness_ ? uroughness_ : roughness_,
                vroughness_ ? vroughness_
                            : (uroughness_ ? uroughness_ : roughness_),
                remap_roughness_),
            MakeReferenceCounted<materials::UberMaterial>(
                reflectance_, transmittance_, diffuse_, specular_, opacity_,
                eta_back_, eta_front_, uroughness_ ? uroughness_ : roughness_,
                vroughness_ ? vroughness_
                            : (uroughness_ ? uroughness_ : roughness_),
                remap_roughness_),
            front_bump, back_bump)) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;

 private:
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> reflectance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular_;
  ReferenceCounted<ValueTexture2D<visual>> opacity_;
  ReferenceCounted<ValueTexture2D<visual>> eta_front_;
  ReferenceCounted<ValueTexture2D<visual>> eta_back_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_;
  ReferenceCounted<ValueTexture2D<visual>> uroughness_;
  ReferenceCounted<ValueTexture2D<visual>> vroughness_;
  bool remap_roughness_;
  const MaterialBuilderResult default_;
};

std::shared_ptr<NestedMaterialBuilder> UberObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<ValueTexture2D<visual>> uroughness_texture;
  ReferenceCounted<ValueTexture2D<visual>> vroughness_texture;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_texture = texture_manager.AllocateUniformReflectorTexture(
          kDefaultTransmittance);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultSpecular);
  ReferenceCounted<ValueTexture2D<visual>> opacity_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultOpacity);
  ReferenceCounted<ValueTexture2D<visual>> eta_back_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaBack);
  ReferenceCounted<ValueTexture2D<visual>> roughness_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultRoughness);
  bool remap_roughness = kDefaultRemapRoughness;

  auto kr = parameters.find("Kr");
  if (kr != parameters.end()) {
    reflectance_texture = kr->second.GetReflectorTextures(1).front();
  }

  auto kt = parameters.find("Kt");
  if (kt != parameters.end()) {
    transmittance_texture = kt->second.GetReflectorTextures(1).front();
  }

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto ks = parameters.find("Ks");
  if (ks != parameters.end()) {
    specular_texture = ks->second.GetReflectorTextures(1).front();
  }

  auto roughness = parameters.find("roughness");
  if (roughness != parameters.end()) {
    roughness_texture = roughness->second.GetFloatTextures(1).front();
  }

  auto uroughness = parameters.find("uroughness");
  if (uroughness != parameters.end()) {
    uroughness_texture = uroughness->second.GetFloatTextures(1).front();
  }

  auto vroughness = parameters.find("vroughness");
  if (vroughness != parameters.end()) {
    vroughness_texture = vroughness->second.GetFloatTextures(1).front();
  }

  auto remaproughness = parameters.find("remaproughness");
  if (remaproughness != parameters.end()) {
    remap_roughness = remaproughness->second.GetBoolValues(1).front();
  }

  auto eta = parameters.find("eta");
  if (eta != parameters.end()) {
    eta_back_texture = eta->second.GetFloatTextures(1).front();
  }

  auto opacity = parameters.find("opacity");
  if (opacity != parameters.end()) {
    opacity_texture = opacity->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<ValueTexture2D<visual>> eta_front_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaFront);

  return std::make_unique<NestedUberObjectBuilder>(
      std::move(reflectance_texture), std::move(transmittance_texture),
      std::move(diffuse_texture), std::move(specular_texture),
      std::move(opacity_texture), std::move(eta_front_texture),
      std::move(eta_back_texture), std::move(roughness_texture),
      std::move(uroughness_texture), std::move(vroughness_texture),
      remap_roughness, std::move(front_normal_map), std::move(back_normal_map));
}

MaterialBuilderResult NestedUberObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<1>(default_),
                           std::get<2>(default_), std::get<3>(default_));
  }

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture = reflectance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_texture = transmittance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture = diffuse_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture = specular_;
  ReferenceCounted<ValueTexture2D<visual>> opacity_texture = opacity_;
  ReferenceCounted<ValueTexture2D<visual>> eta_back_texture = eta_back_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_texture = roughness_;
  ReferenceCounted<ValueTexture2D<visual>> uroughness_texture = uroughness_;
  ReferenceCounted<ValueTexture2D<visual>> vroughness_texture = vroughness_;
  bool remap_roughness = remap_roughness_;
  ReferenceCounted<NormalMap> front_normal_map = std::get<2>(default_);
  ReferenceCounted<NormalMap> back_normal_map = std::get<3>(default_);

  auto kr = parameters.find("Kr");
  if (kr != parameters.end()) {
    reflectance_texture = kr->second.GetReflectorTextures(1).front();
  }

  auto kt = parameters.find("Kt");
  if (kt != parameters.end()) {
    transmittance_texture = kt->second.GetReflectorTextures(1).front();
  }

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto ks = parameters.find("Ks");
  if (ks != parameters.end()) {
    specular_texture = ks->second.GetReflectorTextures(1).front();
  }

  auto roughness = parameters.find("roughness");
  if (roughness != parameters.end()) {
    roughness_texture = roughness->second.GetFloatTextures(1).front();
  }

  auto uroughness = parameters.find("uroughness");
  if (uroughness != parameters.end()) {
    uroughness_texture = uroughness->second.GetFloatTextures(1).front();
  }

  auto vroughness = parameters.find("vroughness");
  if (vroughness != parameters.end()) {
    vroughness_texture = vroughness->second.GetFloatTextures(1).front();
  }

  auto remaproughness = parameters.find("remaproughness");
  if (remaproughness != parameters.end()) {
    remap_roughness = remaproughness->second.GetBoolValues(1).front();
  }

  auto eta = parameters.find("eta");
  if (eta != parameters.end()) {
    eta_back_texture = eta->second.GetFloatTextures(1).front();
  }

  auto opacity = parameters.find("opacity");
  if (opacity != parameters.end()) {
    opacity_texture = opacity->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<Material> front_material =
      MakeReferenceCounted<materials::UberMaterial>(
          reflectance_texture, transmittance_texture, diffuse_texture,
          specular_texture, opacity_texture, eta_front_, eta_back_texture,
          uroughness_texture ? uroughness_texture : roughness_texture,
          vroughness_texture
              ? vroughness_texture
              : (uroughness_texture ? uroughness_texture : roughness_texture),
          remap_roughness);
  ReferenceCounted<Material> back_material =
      MakeReferenceCounted<materials::UberMaterial>(
          std::move(reflectance_texture), std::move(transmittance_texture),
          std::move(diffuse_texture), std::move(specular_texture),
          std::move(opacity_texture), std::move(eta_back_texture), eta_front_,
          uroughness_texture ? uroughness_texture : roughness_texture,
          vroughness_texture
              ? vroughness_texture
              : (uroughness_texture ? uroughness_texture : roughness_texture),
          remap_roughness);

  return std::make_tuple(std::move(front_material), std::move(back_material),
                         std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_uber_builder =
    std::make_unique<UberObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris