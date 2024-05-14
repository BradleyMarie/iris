#include "frontends/pbrt/materials/glass.h"

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/glass_material.h"
#include "iris/reflectors/uniform_reflector.h"
#include "iris/textures/constant_texture.h"

namespace iris::pbrt_frontend::materials {
namespace {

static const iris::visual kDefaultEtaIncident = 1.0;
static const iris::visual kDefaultEtaTransmitted = 1.5;
static const iris::visual kDefaultReflectance = 1.0;
static const iris::visual kDefaultTransmittance = 1.0;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"eta", Parameter::FLOAT_TEXTURE},
        {"Kr", Parameter::REFLECTOR_TEXTURE},
        {"Kt", Parameter::REFLECTOR_TEXTURE},
};

class GlassObjectBuilder
    : public ObjectBuilder<
          std::shared_ptr<ObjectBuilder<
              std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                         ReferenceCounted<NormalMap>,
                         ReferenceCounted<NormalMap>>,
              TextureManager&>>,
          TextureManager&> {
 public:
  GlassObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<ObjectBuilder<
      std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                 ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
      TextureManager&>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        TextureManager& texture_manager) const override;
};

class NestedGlassObjectBuilder
    : public ObjectBuilder<
          std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                     ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
          TextureManager&> {
 public:
  NestedGlassObjectBuilder(
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          reflectance,
      iris::ReferenceCounted<iris::textures::PointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>
          transmitttance,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          eta_incident,
      iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
          eta_transmitted,
      iris::ReferenceCounted<iris::NormalMap> front_normal_map,
      iris::ReferenceCounted<iris::NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        reflectance_(std::move(reflectance)),
        transmittance_(std::move(transmitttance)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        default_(std::make_tuple(
            iris::MakeReferenceCounted<iris::materials::GlassMaterial>(
                reflectance_, transmittance_, eta_incident_, eta_transmitted_),
            iris::MakeReferenceCounted<iris::materials::GlassMaterial>(
                reflectance_, transmittance_, eta_transmitted_, eta_incident_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

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
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      eta_incident_;
  iris::ReferenceCounted<iris::textures::ValueTexture2D<iris::visual>>
      eta_transmitted_;
  std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
             ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<ObjectBuilder<
    std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
               ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
    TextureManager&>>
GlassObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  auto reflectance_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultReflectance);
  auto transmittance_texture =
      texture_manager.AllocateUniformReflectorTexture(kDefaultTransmittance);
  auto eta_incident_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaIncident);
  auto eta_transmitted_texture =
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
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_unique<NestedGlassObjectBuilder>(
      std::move(reflectance_texture), std::move(transmittance_texture),
      std::move(eta_incident_texture), std::move(eta_transmitted_texture),
      std::move(front_normal_map), std::move(back_normal_map));
}

std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
           ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>
NestedGlassObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    TextureManager& texture_manager) const {
  if (parameters.empty()) {
    return default_;
  }

  auto reflectance_texture = reflectance_;
  auto transmittance_texture = transmittance_;
  auto eta_incident_texture = eta_incident_;
  auto eta_transmitted_texture = eta_transmitted_;
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

  auto eta = parameters.find("eta");
  if (eta != parameters.end()) {
    eta_transmitted_texture = eta->second.GetFloatTextures(1).front();
  }

  auto bump = parameters.find("bumpmap");
  if (bump != parameters.end()) {
    auto normal_maps = MakeBumpMap(bump->second.GetFloatTextures(1).front());
    front_normal_map = normal_maps.first;
    back_normal_map = normal_maps.second;
  }

  return std::make_tuple(
      iris::MakeReferenceCounted<iris::materials::GlassMaterial>(
          reflectance_texture, transmittance_texture, eta_incident_texture,
          eta_transmitted_texture),
      iris::MakeReferenceCounted<iris::materials::GlassMaterial>(
          reflectance_texture, transmittance_texture, eta_transmitted_texture,
          eta_incident_texture),
      std::move(front_normal_map), std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>>
    g_glass_builder = std::make_unique<GlassObjectBuilder>();

}  // namespace iris::pbrt_frontend::materials