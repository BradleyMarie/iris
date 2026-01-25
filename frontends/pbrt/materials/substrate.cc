#include "frontends/pbrt/materials/substrate.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/substrate_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeSubstrateMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

MaterialResult MakeSubstrate(const Material::Substrate& substrate,
                             const Shape::MaterialOverrides& overrides,
                             TextureManager& texture_manager) {
  Material::Substrate with_defaults = Defaults().materials().substrate();
  with_defaults.MergeFrom(substrate);
  with_defaults.MergeFromString(overrides.SerializeAsString());

  ReferenceCounted<iris::Material> material = MakeSubstrateMaterial(
      texture_manager.AllocateReflectorTexture(with_defaults.kd()),
      texture_manager.AllocateReflectorTexture(with_defaults.ks()),
      texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
      texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
      with_defaults.remaproughness());

  return MaterialResult{{material, material},
                        MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
