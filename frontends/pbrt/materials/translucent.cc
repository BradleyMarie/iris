#include "frontends/pbrt/materials/translucent.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/translucent_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::TranslucentMaterial;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

constexpr visual kDefaultDiffuse = 0.25;
constexpr visual kDefaultEtaIncident = 1.0;
constexpr visual kDefaultEtaTransmitted = 1.5;
constexpr visual kDefaultSpecular = 0.25;
constexpr visual kDefaultReflectance = 0.5;
constexpr visual kDefaultRoughness = 0.1;
constexpr bool kDefaultRemapRoughness = true;
constexpr visual kDefaultTransmittance = 0.5;

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

class TranslucentObjectBuilder : public MaterialBuilder {
 public:
  TranslucentObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager,
      TextureManager& texture_manager) const override;
};

class NestedTranslucentObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedTranslucentObjectBuilder(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          transmittance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
      ReferenceCounted<ValueTexture2D<visual>> eta_incident,
      ReferenceCounted<ValueTexture2D<visual>> eta_transmitted,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
      ReferenceCounted<ValueTexture2D<visual>> roughness, bool remap_roughness,
      ReferenceCounted<NormalMap> front_bump,
      ReferenceCounted<NormalMap> back_bump)
      : ObjectBuilder(g_parameters),
        reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmittance)),
        diffuse_(std::move(diffuse)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        specular_(std::move(specular)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness),
        default_(std::make_tuple(
            MakeReferenceCounted<TranslucentMaterial>(
                reflectance_, transmittance_, diffuse_, specular_,
                eta_incident_, eta_transmitted_, roughness_, remap_roughness_),
            MakeReferenceCounted<TranslucentMaterial>(
                reflectance_, transmittance_, diffuse_, specular_,
                eta_transmitted_, eta_incident_, roughness_, remap_roughness_),
            front_bump, back_bump)) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager,
      TextureManager& texture_manager) const override;

 private:
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  const ReferenceCounted<ValueTexture2D<visual>> eta_incident_;
  const ReferenceCounted<ValueTexture2D<visual>> eta_transmitted_;
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  const ReferenceCounted<ValueTexture2D<visual>> roughness_;
  bool remap_roughness_;
  const MaterialBuilderResult default_;
};

std::shared_ptr<NestedMaterialBuilder> TranslucentObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultSpecular);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_texture = texture_manager.AllocateUniformReflectorTexture(
          kDefaultTransmittance);
  ReferenceCounted<ValueTexture2D<visual>> roughness_texture =
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
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<ValueTexture2D<visual>> eta_incident_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaIncident);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaTransmitted);

  return std::make_unique<NestedTranslucentObjectBuilder>(
      std::move(reflectance_texture), std::move(transmittance_texture),
      std::move(diffuse_texture), std::move(eta_incident_texture),
      std::move(eta_transmitted_texture), std::move(specular_texture),
      std::move(roughness_texture), remap_roughness,
      std::move(front_normal_map), std::move(back_normal_map));
}

MaterialBuilderResult NestedTranslucentObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return default_;
  }

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture = diffuse_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture = specular_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture = reflectance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_texture = transmittance_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_texture = roughness_;
  bool remap_roughness = remap_roughness_;
  ReferenceCounted<NormalMap> front_normal_map = std::get<2>(default_);
  ReferenceCounted<NormalMap> back_normal_map = std::get<3>(default_);

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
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  ReferenceCounted<Material> front_material =
      MakeReferenceCounted<TranslucentMaterial>(
          reflectance_texture, transmittance_texture, diffuse_texture,
          specular_texture, eta_incident_, eta_transmitted_, roughness_texture,
          remap_roughness);
  ReferenceCounted<Material> back_material =
      MakeReferenceCounted<TranslucentMaterial>(
          std::move(reflectance_texture), std::move(transmittance_texture),
          std::move(diffuse_texture), std::move(specular_texture),
          eta_transmitted_, eta_incident_, std::move(roughness_texture),
          remap_roughness);

  return std::make_tuple(front_material, back_material,
                         std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_translucent_builder =
    std::make_unique<TranslucentObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris