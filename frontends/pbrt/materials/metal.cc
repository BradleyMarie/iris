#include "frontends/pbrt/materials/metal.h"

#include <map>

#include "frontends/pbrt/materials/bumpmap.h"
#include "iris/materials/metal_material.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MetalMaterial;
using ::iris::textures::PointerTexture2D;
using ::iris::textures::ValueTexture2D;

class DefaultEta : public Spectrum {
 public:
  visual_t Intensity(visual_t wavelength) const override {
    return static_cast<visual_t>(1.0);
  }
};

static const ReferenceCounted<Spectrum> kDefaultEtaIncident =
    MakeReferenceCounted<DefaultEta>();

static const std::map<visual, visual> kCopperEtaTransmitted{
    {static_cast<visual>(298.7570554), static_cast<visual>(1.400313)},
    {static_cast<visual>(302.4004341), static_cast<visual>(1.380000)},
    {static_cast<visual>(306.1337728), static_cast<visual>(1.358438)},
    {static_cast<visual>(309.9604450), static_cast<visual>(1.340000)},
    {static_cast<visual>(313.8839949), static_cast<visual>(1.329063)},
    {static_cast<visual>(317.9081487), static_cast<visual>(1.325000)},
    {static_cast<visual>(322.0368260), static_cast<visual>(1.332500)},
    {static_cast<visual>(326.2741526), static_cast<visual>(1.340000)},
    {static_cast<visual>(330.6244747), static_cast<visual>(1.334375)},
    {static_cast<visual>(335.0923730), static_cast<visual>(1.325000)},
    {static_cast<visual>(339.6826795), static_cast<visual>(1.317812)},
    {static_cast<visual>(344.4004944), static_cast<visual>(1.310000)},
    {static_cast<visual>(349.2512056), static_cast<visual>(1.300313)},
    {static_cast<visual>(354.2405086), static_cast<visual>(1.290000)},
    {static_cast<visual>(359.3744290), static_cast<visual>(1.281563)},
    {static_cast<visual>(364.6593471), static_cast<visual>(1.270000)},
    {static_cast<visual>(370.1020239), static_cast<visual>(1.249062)},
    {static_cast<visual>(375.7096303), static_cast<visual>(1.225000)},
    {static_cast<visual>(381.4897785), static_cast<visual>(1.200000)},
    {static_cast<visual>(387.4505563), static_cast<visual>(1.180000)},
    {static_cast<visual>(393.6005651), static_cast<visual>(1.174375)},
    {static_cast<visual>(399.9489613), static_cast<visual>(1.175000)},
    {static_cast<visual>(406.5055016), static_cast<visual>(1.177500)},
    {static_cast<visual>(413.2805933), static_cast<visual>(1.180000)},
    {static_cast<visual>(420.2853492), static_cast<visual>(1.178125)},
    {static_cast<visual>(427.5316483), static_cast<visual>(1.175000)},
    {static_cast<visual>(435.0322035), static_cast<visual>(1.172812)},
    {static_cast<visual>(442.8006357), static_cast<visual>(1.170000)},
    {static_cast<visual>(450.8515564), static_cast<visual>(1.165312)},
    {static_cast<visual>(459.2006593), static_cast<visual>(1.160000)},
    {static_cast<visual>(467.8648226), static_cast<visual>(1.155312)},
    {static_cast<visual>(476.8622231), static_cast<visual>(1.150000)},
    {static_cast<visual>(486.2124627), static_cast<visual>(1.142812)},
    {static_cast<visual>(495.9367120), static_cast<visual>(1.135000)},
    {static_cast<visual>(506.0578694), static_cast<visual>(1.131562)},
    {static_cast<visual>(516.6007417), static_cast<visual>(1.120000)},
    {static_cast<visual>(527.5922468), static_cast<visual>(1.092437)},
    {static_cast<visual>(539.0616435), static_cast<visual>(1.040000)},
    {static_cast<visual>(551.0407911), static_cast<visual>(0.950375)},
    {static_cast<visual>(563.5644455), static_cast<visual>(0.826000)},
    {static_cast<visual>(576.6705953), static_cast<visual>(0.645875)},
    {static_cast<visual>(590.4008476), static_cast<visual>(0.468000)},
    {static_cast<visual>(604.8008683), static_cast<visual>(0.351250)},
    {static_cast<visual>(619.9208900), static_cast<visual>(0.272000)},
    {static_cast<visual>(635.8162974), static_cast<visual>(0.230813)},
    {static_cast<visual>(652.5483053), static_cast<visual>(0.214000)},
    {static_cast<visual>(670.1847459), static_cast<visual>(0.209250)},
    {static_cast<visual>(688.8009889), static_cast<visual>(0.213000)},
    {static_cast<visual>(708.4810171), static_cast<visual>(0.216250)},
    {static_cast<visual>(729.3186941), static_cast<visual>(0.223000)},
    {static_cast<visual>(751.4192606), static_cast<visual>(0.236500)},
    {static_cast<visual>(774.9011125), static_cast<visual>(0.250000)},
    {static_cast<visual>(799.8979226), static_cast<visual>(0.254188)},
    {static_cast<visual>(826.5611867), static_cast<visual>(0.260000)},
    {static_cast<visual>(855.0632966), static_cast<visual>(0.280000)},
    {static_cast<visual>(885.6012714), static_cast<visual>(0.300000)}};

static const std::map<visual, visual> kCopperK = {
    {static_cast<visual>(298.7570554), static_cast<visual>(1.687000)},
    {static_cast<visual>(302.4004341), static_cast<visual>(1.703313)},
    {static_cast<visual>(306.1337728), static_cast<visual>(1.720000)},
    {static_cast<visual>(309.9604450), static_cast<visual>(1.744563)},
    {static_cast<visual>(313.8839949), static_cast<visual>(1.770000)},
    {static_cast<visual>(317.9081487), static_cast<visual>(1.791625)},
    {static_cast<visual>(322.0368260), static_cast<visual>(1.810000)},
    {static_cast<visual>(326.2741526), static_cast<visual>(1.822125)},
    {static_cast<visual>(330.6244747), static_cast<visual>(1.834000)},
    {static_cast<visual>(335.0923730), static_cast<visual>(1.851750)},
    {static_cast<visual>(339.6826795), static_cast<visual>(1.872000)},
    {static_cast<visual>(344.4004944), static_cast<visual>(1.894250)},
    {static_cast<visual>(349.2512056), static_cast<visual>(1.916000)},
    {static_cast<visual>(354.2405086), static_cast<visual>(1.931688)},
    {static_cast<visual>(359.3744290), static_cast<visual>(1.950000)},
    {static_cast<visual>(364.6593471), static_cast<visual>(1.972438)},
    {static_cast<visual>(370.1020239), static_cast<visual>(2.015000)},
    {static_cast<visual>(375.7096303), static_cast<visual>(2.121562)},
    {static_cast<visual>(381.4897785), static_cast<visual>(2.210000)},
    {static_cast<visual>(387.4505563), static_cast<visual>(2.177188)},
    {static_cast<visual>(393.6005651), static_cast<visual>(2.130000)},
    {static_cast<visual>(399.9489613), static_cast<visual>(2.160063)},
    {static_cast<visual>(406.5055016), static_cast<visual>(2.210000)},
    {static_cast<visual>(413.2805933), static_cast<visual>(2.249938)},
    {static_cast<visual>(420.2853492), static_cast<visual>(2.289000)},
    {static_cast<visual>(427.5316483), static_cast<visual>(2.326000)},
    {static_cast<visual>(435.0322035), static_cast<visual>(2.362000)},
    {static_cast<visual>(442.8006357), static_cast<visual>(2.397625)},
    {static_cast<visual>(450.8515564), static_cast<visual>(2.433000)},
    {static_cast<visual>(459.2006593), static_cast<visual>(2.469187)},
    {static_cast<visual>(467.8648226), static_cast<visual>(2.504000)},
    {static_cast<visual>(476.8622231), static_cast<visual>(2.535875)},
    {static_cast<visual>(486.2124627), static_cast<visual>(2.564000)},
    {static_cast<visual>(495.9367120), static_cast<visual>(2.589625)},
    {static_cast<visual>(506.0578694), static_cast<visual>(2.605000)},
    {static_cast<visual>(516.6007417), static_cast<visual>(2.595562)},
    {static_cast<visual>(527.5922468), static_cast<visual>(2.583000)},
    {static_cast<visual>(539.0616435), static_cast<visual>(2.576500)},
    {static_cast<visual>(551.0407911), static_cast<visual>(2.599000)},
    {static_cast<visual>(563.5644455), static_cast<visual>(2.678062)},
    {static_cast<visual>(576.6705953), static_cast<visual>(2.809000)},
    {static_cast<visual>(590.4008476), static_cast<visual>(3.010750)},
    {static_cast<visual>(604.8008683), static_cast<visual>(3.240000)},
    {static_cast<visual>(619.9208900), static_cast<visual>(3.458187)},
    {static_cast<visual>(635.8162974), static_cast<visual>(3.670000)},
    {static_cast<visual>(652.5483053), static_cast<visual>(3.863125)},
    {static_cast<visual>(670.1847459), static_cast<visual>(4.050000)},
    {static_cast<visual>(688.8009889), static_cast<visual>(4.239563)},
    {static_cast<visual>(708.4810171), static_cast<visual>(4.430000)},
    {static_cast<visual>(729.3186941), static_cast<visual>(4.619563)},
    {static_cast<visual>(751.4192606), static_cast<visual>(4.817000)},
    {static_cast<visual>(774.9011125), static_cast<visual>(5.034125)},
    {static_cast<visual>(799.8979226), static_cast<visual>(5.260000)},
    {static_cast<visual>(826.5611867), static_cast<visual>(5.485625)},
    {static_cast<visual>(855.0632966), static_cast<visual>(5.717000)},
    {static_cast<visual>(885.6012714), static_cast<visual>(0.300000)}};

constexpr visual kDefaultRoughness = 0.01;
constexpr bool kDefaultRemapRoughness = true;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"bumpmap", Parameter::FLOAT_TEXTURE},
        {"eta", Parameter::SPECTRUM},
        {"k", Parameter::SPECTRUM},
        {"roughness", Parameter::FLOAT_TEXTURE},
        {"uroughness", Parameter::FLOAT_TEXTURE},
        {"vroughness", Parameter::FLOAT_TEXTURE},
        {"remaproughness", Parameter::BOOL},
};

class MetalObjectBuilder : public MaterialBuilder {
 public:
  MetalObjectBuilder() noexcept : ObjectBuilder(g_parameters) {}

  std::shared_ptr<NestedMaterialBuilder> Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;
};

class NestedMetalObjectBuilder : public NestedMaterialBuilder {
 public:
  NestedMetalObjectBuilder(ReferenceCounted<Spectrum> k,
                           ReferenceCounted<Spectrum> eta_incident,
                           ReferenceCounted<Spectrum> eta_transmitted,
                           ReferenceCounted<ValueTexture2D<visual>> roughness,
                           ReferenceCounted<ValueTexture2D<visual>> roughness_u,
                           ReferenceCounted<ValueTexture2D<visual>> roughness_v,
                           bool remap_roughness,
                           ReferenceCounted<NormalMap> front_normal_map,
                           ReferenceCounted<NormalMap> back_normal_map)
      : ObjectBuilder(g_parameters),
        k_(std::move(k)),
        eta_incident_(std::move(eta_incident)),
        eta_transmitted_(std::move(eta_transmitted)),
        roughness_(std::move(roughness)),
        roughness_u_(std::move(roughness_u)),
        roughness_v_(std::move(roughness_v)),
        remap_roughness_(remap_roughness),
        default_(std::make_tuple(
            MakeReferenceCounted<MetalMaterial>(
                k_, eta_incident_, eta_transmitted_,
                roughness_u_ ? roughness_u_ : roughness_,
                roughness_v_ ? roughness_v_
                             : (roughness_u_ ? roughness_u_ : roughness_),
                remap_roughness_),
            std::move(front_normal_map), std::move(back_normal_map))) {}

  MaterialBuilderResult Build(
      const std::unordered_map<std::string_view, Parameter>& parameters,
      const MaterialManager& material_manager, TextureManager& texture_manager,
      SpectrumManager& spectrum_manager) const override;

 private:
  ReferenceCounted<Spectrum> k_;
  ReferenceCounted<Spectrum> eta_incident_;
  ReferenceCounted<Spectrum> eta_transmitted_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_u_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_v_;
  bool remap_roughness_;
  const std::tuple<ReferenceCounted<Material>, ReferenceCounted<NormalMap>,
                   ReferenceCounted<NormalMap>>
      default_;
};

std::shared_ptr<NestedMaterialBuilder> MetalObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  ReferenceCounted<NormalMap> front_normal_map;
  ReferenceCounted<NormalMap> back_normal_map;
  ReferenceCounted<ValueTexture2D<visual>> roughness_u;
  ReferenceCounted<ValueTexture2D<visual>> roughness_v;
  ReferenceCounted<Spectrum> k_spectra =
      spectrum_manager.AllocateSpectrum(kCopperK);
  ReferenceCounted<Spectrum> eta_transmitted =
      spectrum_manager.AllocateSpectrum(kCopperEtaTransmitted);
  ReferenceCounted<ValueTexture2D<visual>> roughness_texture =
      texture_manager.AllocateUniformFloatTexture(kDefaultRoughness);
  bool remap_roughness = kDefaultRemapRoughness;

  auto eta = parameters.find("eta");
  if (eta != parameters.end()) {
    eta_transmitted = eta->second.GetSpectra(1).front();
  }

  auto k = parameters.find("k");
  if (k != parameters.end()) {
    k_spectra = k->second.GetSpectra(1).front();
  }

  auto roughness = parameters.find("roughness");
  if (roughness != parameters.end()) {
    roughness_texture = roughness->second.GetFloatTextures(1).front();
  }

  auto uroughness = parameters.find("uroughness");
  if (uroughness != parameters.end()) {
    roughness_u = uroughness->second.GetFloatTextures(1).front();
  }

  auto vroughness = parameters.find("vroughness");
  if (vroughness != parameters.end()) {
    roughness_v = vroughness->second.GetFloatTextures(1).front();
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

  return std::make_unique<NestedMetalObjectBuilder>(
      std::move(k_spectra), kDefaultEtaIncident, std::move(eta_transmitted),
      std::move(roughness_texture), std::move(roughness_u),
      std::move(roughness_v), remap_roughness, std::move(front_normal_map),
      std::move(back_normal_map));
}

MaterialBuilderResult NestedMetalObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MaterialManager& material_manager, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager) const {
  if (parameters.empty()) {
    return std::make_tuple(std::get<0>(default_), std::get<0>(default_),
                           std::get<1>(default_), std::get<2>(default_));
  }

  ReferenceCounted<Spectrum> k_spectra = k_;
  ReferenceCounted<Spectrum> eta_transmitted = eta_transmitted_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_texture = roughness_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_u = roughness_u_;
  ReferenceCounted<ValueTexture2D<visual>> roughness_v = roughness_v_;
  ReferenceCounted<NormalMap> front_normal_map = std::get<1>(default_);
  ReferenceCounted<NormalMap> back_normal_map = std::get<2>(default_);
  bool remap_roughness = remap_roughness_;

  auto eta = parameters.find("eta");
  if (eta != parameters.end()) {
    eta_transmitted = eta->second.GetSpectra(1).front();
  }

  auto k = parameters.find("k");
  if (k != parameters.end()) {
    k_spectra = k->second.GetSpectra(1).front();
  }

  auto roughness = parameters.find("roughness");
  if (roughness != parameters.end()) {
    roughness_texture = roughness->second.GetFloatTextures(1).front();
  }

  auto uroughness = parameters.find("uroughness");
  if (uroughness != parameters.end()) {
    roughness_u = uroughness->second.GetFloatTextures(1).front();
  }

  auto vroughness = parameters.find("vroughness");
  if (vroughness != parameters.end()) {
    roughness_v = vroughness->second.GetFloatTextures(1).front();
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

  ReferenceCounted<Material> material = MakeReferenceCounted<MetalMaterial>(
      k_spectra, eta_incident_, eta_transmitted,
      roughness_u ? roughness_u : roughness_texture,
      roughness_v ? roughness_v
                  : (roughness_u ? roughness_u : roughness_texture),
      remap_roughness);

  return std::make_tuple(material, material, std::move(front_normal_map),
                         std::move(back_normal_map));
}

}  // namespace

const std::unique_ptr<const MaterialBuilder> g_metal_builder =
    std::make_unique<MetalObjectBuilder>();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris