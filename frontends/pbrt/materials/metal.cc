#include "frontends/pbrt/materials/metal.h"

#include <cstdlib>
#include <iostream>
#include <map>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/metal_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MakeMetalMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::Spectrum;
using ::pbrt_proto::v3::SpectrumTextureParameter;

constexpr visual kDefaultEtaConductor = 1.0;

Spectrum ToSpectrum(const SpectrumTextureParameter& parameter) {
  Spectrum result;
  switch (parameter.spectrum_texture_parameter_type_case()) {
    case SpectrumTextureParameter::kUniformSpectrum:
      result.set_uniform_spectrum(parameter.uniform_spectrum());
      break;
    case SpectrumTextureParameter::kBlackbodySpectrum:
      *result.mutable_blackbody_spectrum() = parameter.blackbody_spectrum();
      break;
    case SpectrumTextureParameter::kRgbSpectrum:
      *result.mutable_rgb_spectrum() = parameter.rgb_spectrum();
      break;
    case SpectrumTextureParameter::kXyzSpectrum:
      *result.mutable_xyz_spectrum() = parameter.xyz_spectrum();
      break;
    case SpectrumTextureParameter::kSampledSpectrum:
      *result.mutable_sampled_spectrum() = parameter.sampled_spectrum();
      break;
    case SpectrumTextureParameter::kSampledSpectrumFilename:
      result.set_sampled_spectrum_filename(
          parameter.sampled_spectrum_filename());
      break;
    case SpectrumTextureParameter::kSpectrumTextureName:
      std::cerr << "ERROR: Spectrum texture parameters are not implemented for "
                   "metal Material"
                << std::endl;
      exit(EXIT_FAILURE);
    case SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET:
      break;
  }
  return result;
}

}  // namespace

MaterialResult MakeMetal(const Material::Metal& metal,
                         const Shape::MaterialOverrides& overrides,
                         TextureManager& texture_manager,
                         SpectrumManager& spectrum_manager) {
  Material::Metal with_defaults = Defaults().materials().metal();
  with_defaults.MergeFrom(metal);
  with_defaults.MergeFromString(overrides.SerializeAsString());

  if (with_defaults.uroughness().float_texture_parameter_type_case() ==
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    *with_defaults.mutable_uroughness() = with_defaults.roughness();
  }

  if (with_defaults.vroughness().float_texture_parameter_type_case() ==
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    *with_defaults.mutable_vroughness() = with_defaults.roughness();
  }

  ReferenceCounted<iris::Material> material = MakeMetalMaterial(
      texture_manager.AllocateFloatTexture(kDefaultEtaConductor),
      spectrum_manager.AllocateSpectrum(ToSpectrum(with_defaults.eta())),
      spectrum_manager.AllocateSpectrum(ToSpectrum(with_defaults.k())),
      texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
      texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
      with_defaults.remaproughness());

  return MaterialResult{{material, material},
                        MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
