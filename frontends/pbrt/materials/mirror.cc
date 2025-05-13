#include "frontends/pbrt/materials/mirror.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/mirror_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeMirrorMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

MaterialResult MakeMirror(const Material::Mirror& mirror,
                          const Shape::MaterialOverrides& overrides,
                          TextureManager& texture_manager) {
  Material::Mirror with_defaults = Defaults().materials().mirror();
  with_defaults.MergeFrom(mirror);

  if (overrides.bumpmap().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_bumpmap()->MergeFrom(overrides.bumpmap());
  }

  if (overrides.kr().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_kr()->MergeFrom(overrides.kr());
  }

  return MaterialResult{
      MakeMirrorMaterial(
          texture_manager.AllocateReflectorTexture(with_defaults.kr())),
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
