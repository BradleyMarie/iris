#include "frontends/pbrt/materials/plastic.h"

#include <algorithm>
#include <utility>

#include "absl/flags/flag.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/plastic_material.h"

ABSL_FLAG(bool, reverse_plastic_eta, true,
          "If true, the eta of the faces of a plastic material are reversed. "
          "This replicates a bug that exists in pbrt-v3.");

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::PlasticMaterial;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

constexpr visual kDefaultDiffuse = 0.25;
constexpr visual kDefaultEtaFront = 1.0;
constexpr visual kDefaultEtaBack = 1.5;
constexpr visual kDefaultSpecular = 0.25;
constexpr visual kDefaultRoughness = 0.1;
constexpr bool kDefaultRemapRoughness = true;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"Kd", Parameter::REFLECTOR_TEXTURE},
        {"Ks", Parameter::REFLECTOR_TEXTURE},
        {"roughness", Parameter::FLOAT_TEXTURE},
        {"remaproughness", Parameter::BOOL},
};

class PlasticObjectBuilder : public MaterialBuilder {
 public:
  PlasticObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager&, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;
};

class NestedPlasticObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedPlasticObjectBuilder(
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> diffuse,
      ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>> specular,
      ReferenceCounted<ValueTexture2D<visual>> eta_front,
      ReferenceCounted<ValueTexture2D<visual>> eta_back,
      ReferenceCounted<ValueTexture2D<visual>> roughness, bool remap_roughness,
      ReferenceCounted<NormalMap> front_bump,
      ReferenceCounted<NormalMap> back_bump)
      : ObjectBuilder(g_parameters),
        diffuse_(std::move(diffuse)),
        specular_(std::move(specular)),
        eta_front_(std::move(eta_front)),
        eta_back_(std::move(eta_back)),
        roughness_(std::move(roughness)),
        remap_roughness_(remap_roughness),
        default_(std::make_tuple(MakeReferenceCounted<PlasticMaterial>(
                                     diffuse_, specular_, eta_front_, eta_back_,
                                     roughness_, remap_roughness_),
                                 MakeReferenceCounted<PlasticMaterial>(
                                     diffuse_, specular_, eta_back_, eta_front_,
                                     roughness_, remap_roughness_),
                                 front_bump, back_bump)) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;

 private:
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_;
  const ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_;
  const ReferenceCounted<ValueTexture2D<visual>> eta_front_;
  const ReferenceCounted<ValueTexture2D<visual>> eta_back_;
  const ReferenceCounted<ValueTexture2D<visual>> roughness_;
  bool remap_roughness_;
  const MaterialBuilderResult default_;
};

std::shared_ptr<NestedMaterialBuilder> PlasticObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultDiffuse);
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture =
          texture_manager.AllocateUniformReflectorTexture(kDefaultSpecular);
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

  ReferenceCounted<ValueTexture2D<visual>> eta_front =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaFront);
  ReferenceCounted<ValueTexture2D<visual>> eta_back =
      texture_manager.AllocateUniformFloatTexture(kDefaultEtaBack);
  if (absl::GetFlag(FLAGS_reverse_plastic_eta)) {
    std::swap(eta_front, eta_back);
  }

  return std::make_unique<NestedPlasticObjectBuilder>(
      std::move(diffuse_texture), std::move(specular_texture),
      std::move(eta_front), std::move(eta_back), std::move(roughness_texture),
      remap_roughness, std::move(front_normal_map), std::move(back_normal_map));
}

MaterialBuilderResult NestedPlasticObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<1>(default_),
                           std::get<2>(default_), std::get<3>(default_));
  }

  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      diffuse_texture = diffuse_;
  ReferenceCounted<PointerTexture2D<Reflector, SpectralAllocator>>
      specular_texture = specular_;
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
      MakeReferenceCounted<PlasticMaterial>(diffuse_texture, specular_texture,
                                            eta_front_, eta_back_,
                                            roughness_texture, remap_roughness);
  ReferenceCounted<Material> back_material =
      MakeReferenceCounted<PlasticMaterial>(
          std::move(diffuse_texture), std::move(specular_texture), eta_back_,
          eta_front_, std::move(roughness_texture), remap_roughness);

  return std::make_tuple(std::move(front_material), std::move(back_material),
                         std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_plastic_builder =
    std::make_unique<PlasticObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris