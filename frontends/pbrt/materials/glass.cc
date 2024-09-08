#include "frontends/pbrt/materials/glass.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/glass_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::GlassMaterial;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

constexpr visual kDefaultEtaIncident = 1.0;
constexpr visual kDefaultEtaTransmitted = 1.5;
constexpr visual kDefaultReflectance = 1.0;
constexpr visual kDefaultTransmittance = 1.0;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"eta", Parameter::FLOAT_TEXTURE},
        {"Kr", Parameter::REFLECTOR_TEXTURE},
        {"Kt", Parameter::REFLECTOR_TEXTURE},
};

class GlassObjectBuilder : public MaterialBuilder {
 public:
  GlassObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager,
      TextureManager& texture_manager) const override;
};

class NestedGlassObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedGlassObjectBuilder(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          reflectance,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
          transmitttance,
      ReferenceCounted<ValueTexture2D<visual>> eta_incident,
      ReferenceCounted<ValueTexture2D<visual>> eta_transmitted,
      ReferenceCounted<NormalMap> front_normal_map,
      ReferenceCounted<NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmitttance)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        default_(std::make_tuple(
            MakeReferenceCounted<GlassMaterial>(
                reflectance_, transmittance_, eta_incident_, eta_transmitted_),
            MakeReferenceCounted<GlassMaterial>(
                reflectance_, transmittance_, eta_transmitted_, eta_incident_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager,
      TextureManager& texture_manager) const override;

 private:
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_;
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_;
  const ReferenceCounted<ValueTexture2D<visual>> eta_incident_;
  const ReferenceCounted<ValueTexture2D<visual>> eta_transmitted_;
  const MaterialBuilderResult default_;
};

std::shared_ptr<NestedMaterialBuilder> GlassObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_texture = texture_manager.AllocateUniformReflectorTexture(
          kDefaultTransmittance);
  ReferenceCounted<ValueTexture2D<visual>> eta_incident_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaIncident);
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaTransmitted);

  auto kr = parameters.find("Kr");
  if (kr != parameters.end()) {
    reflectance_texture = kr->second.GetReflectorTextures(1).front();
  }

  auto kt = parameters.find("Kt");
  if (kt != parameters.end()) {
    transmittance_texture = kt->second.GetReflectorTextures(1).front();
  }

  auto eta = parameters.find("eta");
  if (eta != parameters.end()) {
    eta_transmitted_texture = eta->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedGlassObjectBuilder>(
      std::move(reflectance_texture), std::move(transmittance_texture),
      std::move(eta_incident_texture), std::move(eta_transmitted_texture),
      std::move(front_normal_map), std::move(back_normal_map));
}

MaterialBuilderResult NestedGlassObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return default_;
  }

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      reflectance_texture = reflectance_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      transmittance_texture = transmittance_;
  ReferenceCounted<ValueTexture2D<visual>> eta_incident_texture = eta_incident_;
  ReferenceCounted<ValueTexture2D<visual>> eta_transmitted_texture =
      eta_transmitted_;
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

  auto eta = parameters.find("eta");
  if (eta != parameters.end()) {
    eta_transmitted_texture = eta->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    std::pair<ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
        normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_tuple(MakeReferenceCounted<GlassMaterial>(
                             reflectance_texture, transmittance_texture,
                             eta_incident_texture, eta_transmitted_texture),
                         MakeReferenceCounted<GlassMaterial>(
                             reflectance_texture, transmittance_texture,
                             eta_transmitted_texture, eta_incident_texture),
                         std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_glass_builder =
    std::make_unique<GlassObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris