#include "frontends/pbrt/materials/glass.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/glass_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeGlassMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

constexpr visual kDefaultEtaFront = 1.0;

MaterialResult MakeGlass(const Material::Glass& glass,
                         const Shape::MaterialOverrides& overrides,
                         TextureManager& texture_manager) {
  Material::Glass with_defaults = Defaults().materials().glass();
  with_defaults.MergeFrom(glass);

  if (overrides.bumpmap().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_bumpmap()->MergeFrom(overrides.bumpmap());
  }

  if (overrides.kr().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_kr()->MergeFrom(overrides.kr());
  }

  if (overrides.kt().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_kt()->MergeFrom(overrides.kt());
  }

  if (overrides.eta().as_float_texture().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_eta()->MergeFrom(overrides.eta().as_float_texture());
  }

  FloatTextureParameter eta_front;
  eta_front.set_float_value(kDefaultEtaFront);

  return MaterialResult{
      MakeGlassMaterial(
          texture_manager.AllocateReflectorTexture(with_defaults.kr()),
          texture_manager.AllocateReflectorTexture(with_defaults.kt()),
          texture_manager.AllocateFloatTexture(eta_front),
          texture_manager.AllocateFloatTexture(with_defaults.eta())),
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
