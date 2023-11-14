#include "frontends/pbrt/materials/plastic.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/plastic_material.h"

namespace iris::pbrt_frontend::materials {
namespace {

static const iris::visual kDefaultDiffuse = 0.25;
static const iris::visual kDefaultSpecular = 0.25;
static const iris::visual kDefaultRoughness = 0.1;
static const bool kDefaultRemapRoughness = true;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kd", Parameter::REFLECTOR_TEXTURE},
        {"Ks", Parameter::REFLECTOR_TEXTURE},
        {"roughness", Parameter::FLOAT_TEXTURE},
        {"remaproughness", Parameter::BOOL},
};

class PlasticObjectBuilder
    : public ObjectBuilder<
          std::shared_ptr<ObjectBuilder<std::tuple<ReferenceCounted<Material>,
                                                   ReferenceCounted<NormalMap>,
                                                   ReferenceCounted<NormalMap>>,
                                        TextureManager&>>,
          TextureManager&> {
 public:
  PlasticObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<ObjectBuilder<
      std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                 ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;
};

class NestedPlasticObjectBuilder
    : public ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                     ReferenceCounted<NormalMap>>,
          TextureManager&> {
 public:
  NestedPlasticObjectBuilder(
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
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness),
        default_(std::make_tuple(
            iris::MakeReferenceCounted<iris::materials::PlasticMaterial>(
                diffuse_, specular_, roughness_, remap_roughness_),
            front_bump, back_bump)) {}

  std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
             ReferenceCounted<NormalMap>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;

 private:
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
    std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
               ReferenceCounted<NormalMap>>,
    TextureManager&>>
PlasticObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  auto diffuse_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  auto specular_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultSpecular);
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

  return std::make_unique<NestedPlasticObjectBuilder>(
      std::move(diffuse_texture), std::move(specular_texture),
      std::move(roughness_texture), remap_roughness,
      std::move(front_normal_map), std::move(back_normal_map));
}

std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
           ReferenceCounted<NormalMap>>
NestedPlasticObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return default_;
  }

  auto diffuse_texture = diffuse_;
  auto specular_texture = specular_;
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

  return std::make_tuple(
      iris::MakeReferenceCounted<iris::materials::PlasticMaterial>(
          std::move(diffuse_texture), std::move(specular_texture),
          std::move(roughness_texture), remap_roughness),
      std::move(front_normal_map), std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                   ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>>
    g_plastic_builder = std::make_unique<PlasticObjectBuilder>();

}  // namespace iris::pbrt_frontend::materials