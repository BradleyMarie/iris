#include "frontends/pbrt/materials/uber.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/uber_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeUberMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

constexpr visual kDefaultEtaIncident = 1.0;

MaterialResult MakeUber(const Material::Uber& uber,
                        const Shape::MaterialOverrides& overrides,
                        TextureManager& texture_manager) {
  Material::Uber with_defaults = Defaults().materials().uber();
  with_defaults.MergeFrom(uber);

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

  if (overrides.kr().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_kr()->MergeFrom(overrides.kr());
  }

  if (overrides.kt().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_kt()->MergeFrom(overrides.kt());
  }

  if (overrides.roughness().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_roughness()->MergeFrom(overrides.roughness());
  }

  if (overrides.uroughness().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_uroughness()->MergeFrom(overrides.uroughness());
  }

  if (overrides.vroughness().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_vroughness()->MergeFrom(overrides.vroughness());
  }

  if (overrides.eta().as_float_texture().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_eta()->MergeFrom(overrides.eta().as_float_texture());
  }

  if (overrides.opacity().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_opacity()->MergeFrom(overrides.opacity());
  }

  if (overrides.has_remaproughness()) {
    with_defaults.set_remaproughness(overrides.remaproughness());
  }

  if (with_defaults.uroughness().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    *with_defaults.mutable_uroughness() = with_defaults.roughness();
  }

  if (with_defaults.vroughness().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    *with_defaults.mutable_vroughness() = with_defaults.roughness();
  }

  return MaterialResult{
      MakeUberMaterial(
          texture_manager.AllocateReflectorTexture(with_defaults.kr()),
          texture_manager.AllocateReflectorTexture(with_defaults.kt()),
          texture_manager.AllocateReflectorTexture(with_defaults.kd()),
          texture_manager.AllocateReflectorTexture(with_defaults.ks()),
          texture_manager.AllocateFloatTexture(kDefaultEtaIncident),
          texture_manager.AllocateFloatTexture(with_defaults.eta()),
          texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
          texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
          texture_manager.AllocateFloatTexture(with_defaults.opacity()),
          with_defaults.remaproughness()),
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
