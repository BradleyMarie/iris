#include "frontends/pbrt/materials/translucent.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/translucent_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeTranslucentMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

constexpr visual kDefaultEtaIncident = 1.0;
constexpr visual kDefaultEtaTransmitted = 1.5;

MaterialResult MakeTranslucent(const Material::Translucent& translucent,
                               const Shape::MaterialOverrides& overrides,
                               TextureManager& texture_manager) {
  Material::Translucent with_defaults = Defaults().materials().translucent();
  with_defaults.MergeFrom(translucent);

  if (overrides.bumpmap().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_bumpmap()->MergeFrom(overrides.bumpmap());
  }

  if (overrides.kd().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_kd()->MergeFrom(overrides.kd());
  }

  if (overrides.ks().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_ks()->MergeFrom(overrides.ks());
  }

  if (overrides.reflect().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_reflect()->MergeFrom(overrides.reflect());
  }

  if (overrides.transmit().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_transmit()->MergeFrom(overrides.transmit());
  }

  if (overrides.roughness().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_roughness()->MergeFrom(overrides.roughness());
  }

  if (overrides.has_remaproughness()) {
    with_defaults.set_remaproughness(overrides.remaproughness());
  }

  return MaterialResult{
      MakeTranslucentMaterial(
          texture_manager.AllocateReflectorTexture(with_defaults.reflect()),
          texture_manager.AllocateReflectorTexture(with_defaults.transmit()),
          texture_manager.AllocateReflectorTexture(with_defaults.kd()),
          texture_manager.AllocateReflectorTexture(with_defaults.ks()),
          texture_manager.AllocateFloatTexture(kDefaultEtaIncident),
          texture_manager.AllocateFloatTexture(kDefaultEtaTransmitted),
          texture_manager.AllocateFloatTexture(with_defaults.roughness()),
          with_defaults.remaproughness()),
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
