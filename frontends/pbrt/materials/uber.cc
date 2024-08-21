#include "frontends/pbrt/materials/uber.h"

#include <algorithm>
#include <utility>

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/uber_material.h"

namespace iris::pbrt_frontend::materials {
namespace {

static const iris::visual kDefaultDiffuse = 0.25;
static const iris::visual kDefaultEtaFront = 1.0;
static const iris::visual kDefaultEtaBack = 1.5;
static const iris::visual kDefaultOpacity = 1.0;
static const iris::visual kDefaultSpecular = 0.25;
static const iris::visual kDefaultRoughness = 0.1;
static const bool kDefaultRemapRoughness = true;

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

class UberObjectBuilder
    : public ObjectBuilder<
          std::shared_ptr<ObjectBuilder<
              std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                         ReferenceCounted<NormalMap>,
                         ReferenceCounted<NormalMap>>,
              TextureManager&>>,
          TextureManager&> {
 public:
  UberObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<ObjectBuilder<
      std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                 ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;
};

class NestedUberObjectBuilder
    : public ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                     ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
          TextureManager&> {
 public:
  NestedUberObjectBuilder(
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          reflectance,
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          transmittance,
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          diffuse,
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          specular,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          opacity,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          eta_front,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          eta_back,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          roughness,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          uroughness,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          vroughness,
      bool remap_roughness, iris::ReferenceCounted<iris::NormalMap> front_bump,
      iris::ReferenceCounted<iris::NormalMap> back_bump)
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
            iris::MakeReferenceCounted<iris::materials::UberMaterial>(
                reflectance_, transmittance_, diffuse_, specular_, opacity_,
                eta_front_, eta_back_, uroughness_ ? uroughness_ : roughness_,
                vroughness_ ? vroughness_ : roughness_, remap_roughness_),
            iris::MakeReferenceCounted<iris::materials::UberMaterial>(
                reflectance_, transmittance_, diffuse_, specular_, opacity_,
                eta_back_, eta_front_, uroughness_ ? uroughness_ : roughness_,
                vroughness_ ? vroughness_ : roughness_, remap_roughness_),
            front_bump, back_bump)) {}

  std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
             ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;

 private:
  iris::ReferenceCounted<iris::textures::PointerTexture2D<
      iris::Reflector, iris::SpectralAllocator>>
      reflectance_;
  iris::ReferenceCounted<iris::textures::PointerTexture2D<
      iris::Reflector, iris::SpectralAllocator>>
      transmittance_;
  iris::ReferenceCounted<iris::textures::PointerTexture2D<
      iris::Reflector, iris::SpectralAllocator>>
      diffuse_;
  iris::ReferenceCounted<iris::textures::PointerTexture2D<
      iris::Reflector, iris::SpectralAllocator>>
      specular_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>> opacity_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      eta_front_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      eta_back_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      roughness_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      uroughness_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      vroughness_;
  bool remap_roughness_;
  const std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<ObjectBuilder<
    std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
               ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
    TextureManager&>>
UberObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      uroughness_texture;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      vroughness_texture;
  auto reflectance_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  auto transmittance_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  auto diffuse_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  auto specular_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultSpecular);
  auto opacity_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultOpacity);
  auto eta_back_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaBack);
  auto roughness_texture =
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
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  auto eta_front_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaFront);

  return std::make_unique<NestedUberObjectBuilder>(
      std::move(reflectance_texture), std::move(transmittance_texture),
      std::move(diffuse_texture), std::move(specular_texture),
      std::move(opacity_texture), std::move(eta_front_texture),
      std::move(eta_back_texture), std::move(roughness_texture),
      std::move(uroughness_texture), std::move(vroughness_texture),
      remap_roughness, std::move(front_normal_map), std::move(back_normal_map));
}

std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
           ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
NestedUberObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<1>(default_),
                           std::get<2>(default_), std::get<3>(default_));
  }

  auto reflectance_texture = reflectance_;
  auto transmittance_texture = transmittance_;
  auto diffuse_texture = diffuse_;
  auto specular_texture = specular_;
  auto opacity_texture = opacity_;
  auto eta_back_texture = eta_back_;
  auto roughness_texture = roughness_;
  auto uroughness_texture = uroughness_;
  auto vroughness_texture = vroughness_;
  bool remap_roughness = remap_roughness_;
  auto front_normal_map = std::get<2>(default_);
  auto back_normal_map = std::get<3>(default_);

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
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  auto front_material =
      iris::MakeReferenceCounted<iris::materials::UberMaterial>(
          reflectance_texture, transmittance_texture, diffuse_texture,
          specular_texture, opacity_texture, eta_front_, eta_back_texture,
          uroughness_texture ? uroughness_texture : roughness_texture,
          vroughness_texture ? vroughness_texture : roughness_texture,
          remap_roughness);
  auto back_material =
      iris::MakeReferenceCounted<iris::materials::UberMaterial>(
          std::move(reflectance_texture), std::move(transmittance_texture),
          std::move(diffuse_texture), std::move(specular_texture),
          std::move(opacity_texture), std::move(eta_back_texture), eta_front_,
          uroughness_texture ? std::move(uroughness_texture)
                             : roughness_texture,
          vroughness_texture ? std::move(vroughness_texture)
                             : roughness_texture,
          remap_roughness);

  return std::make_tuple(std::move(front_material), std::move(back_material),
                         std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>>
    g_uber_builder = std::make_unique<UberObjectBuilder>();

}  // namespace iris::pbrt_frontend::materials