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
  with_defaults.MergeFromString(overrides.SerializeAsString());

  return MaterialResult{
      {MakeGlassMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.kr()),
           texture_manager.AllocateReflectorTexture(with_defaults.kt()),
           texture_manager.AllocateFloatTexture(kDefaultEtaFront),
           texture_manager.AllocateFloatTexture(with_defaults.eta()),
           texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
           texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
           with_defaults.remaproughness()),
       MakeGlassMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.kr()),
           texture_manager.AllocateReflectorTexture(with_defaults.kt()),
           texture_manager.AllocateFloatTexture(with_defaults.eta()),
           texture_manager.AllocateFloatTexture(kDefaultEtaFront),
           texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
           texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
           with_defaults.remaproughness())},
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
