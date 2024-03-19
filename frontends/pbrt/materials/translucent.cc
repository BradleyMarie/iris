#include "frontends/pbrt/materials/translucent.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/translucent_material.h"

namespace iris::pbrt_frontend::materials {
namespace {

static const iris::visual kDefaultDiffuse = 0.25;
static const iris::visual kDefaultSpecular = 0.25;
static const iris::visual kDefaultReflectance = 0.5;
static const iris::visual kDefaultRoughness = 0.1;
static const bool kDefaultRemapRoughness = true;
static const iris::visual kDefaultTransmittance = 0.5;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kd", Parameter::REFLECTOR_TEXTURE},
        {"Ks", Parameter::REFLECTOR_TEXTURE},
        {"reflect", Parameter::REFLECTOR_TEXTURE},
        {"roughness", Parameter::FLOAT_TEXTURE},
        {"remaproughness", Parameter::BOOL},
        {"transmit", Parameter::REFLECTOR_TEXTURE},
};

class TranslucentObjectBuilder
    : public ObjectBuilder<
          std::shared_ptr<ObjectBuilder<
              std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                         ReferenceCounted<NormalMap>,
                         ReferenceCounted<NormalMap>>,
              TextureManager&>>,
          TextureManager&> {
 public:
  TranslucentObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<ObjectBuilder<
      std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                 ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;
};

class NestedTranslucentObjectBuilder
    : public ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                     ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
          TextureManager&> {
 public:
  NestedTranslucentObjectBuilder(
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
          roughness,
      bool remap_roughness, iris::ReferenceCounted<iris::NormalMap> front_bump,
      iris::ReferenceCounted<iris::NormalMap> back_bump)
      : ObjectBuilder(g_parameters),
        reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness),
        default_(std::make_tuple(
            iris::MakeReferenceCounted<iris::materials::TranslucentMaterial>(
                reflectance_, transmittance_, diffuse_, specular_, roughness_,
                remap_roughness_),
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
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      roughness_;
  bool remap_roughness_;
  std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
             ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<ObjectBuilder<
    std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
               ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
    TextureManager&>>
TranslucentObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  auto diffuse_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  auto specular_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultSpecular);
  auto reflectance_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  auto transmittance_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultTransmittance);
  auto roughness_texture =
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

  auto reflect = parameters.find("reflect");
  if (reflect != parameters.end()) {
    reflectance_texture = reflect->second.GetReflectorTextures(1).front();
  }

  auto transmit = parameters.find("transmit");
  if (transmit != parameters.end()) {
    transmittance_texture = transmit->second.GetReflectorTextures(1).front();
  }

  auto roughness = parameters.find("roughness");
  if (roughness != parameters.end()) {
    roughness_texture = roughness->second.GetFloatTextures(1).front();
  }

  auto remaproughness = parameters.find("remaproughness");
  if (remaproughness != parameters.end()) {
    remap_roughness = remaproughness->second.GetBoolValues(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedTranslucentObjectBuilder>(
      std::move(reflectance_texture), std::move(transmittance_texture),
      std::move(diffuse_texture), std::move(specular_texture),
      std::move(roughness_texture), remap_roughness,
      std::move(front_normal_map), std::move(back_normal_map));
}

std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
           ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
NestedTranslucentObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<0>(default_),
                           std::get<1>(default_), std::get<2>(default_));
  }

  auto diffuse_texture = diffuse_;
  auto specular_texture = specular_;
  auto reflectance_texture = reflectance_;
  auto transmittance_texture = transmittance_;
  auto roughness_texture = roughness_;
  bool remap_roughness = remap_roughness_;
  auto front_normal_map = std::get<1>(default_);
  auto back_normal_map = std::get<2>(default_);

  auto kd = parameters.find("Kd");
  if (kd != parameters.end()) {
    diffuse_texture = kd->second.GetReflectorTextures(1).front();
  }

  auto ks = parameters.find("Ks");
  if (ks != parameters.end()) {
    specular_texture = ks->second.GetReflectorTextures(1).front();
  }

  auto reflect = parameters.find("reflect");
  if (reflect != parameters.end()) {
    reflectance_texture = reflect->second.GetReflectorTextures(1).front();
  }

  auto transmit = parameters.find("transmit");
  if (transmit != parameters.end()) {
    transmittance_texture = transmit->second.GetReflectorTextures(1).front();
  }

  auto roughness = parameters.find("roughness");
  if (roughness != parameters.end()) {
    roughness_texture = roughness->second.GetFloatTextures(1).front();
  }

  auto remaproughness = parameters.find("remaproughness");
  if (remaproughness != parameters.end()) {
    remap_roughness = remaproughness->second.GetBoolValues(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  auto material =
      iris::MakeReferenceCounted<iris::materials::TranslucentMaterial>(
          std::move(reflectance_texture), std::move(transmittance_texture),
          std::move(diffuse_texture), std::move(specular_texture),
          std::move(roughness_texture), remap_roughness);

  return std::make_tuple(material, material, std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>>
    g_translucent_builder = std::make_unique<TranslucentObjectBuilder>();

}  // namespace iris::pbrt_frontend::materials