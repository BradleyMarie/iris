#include "frontends/pbrt/materials/matte.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/matte_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeMatteMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

MaterialResult MakeMatte(const Material::Matte& matte,
                         const Shape::MaterialOverrides& overrides,
                         TextureManager& texture_manager) {
  Material::Matte with_defaults = Defaults().materials().matte();
  with_defaults.MergeFrom(matte);
  with_defaults.MergeFromString(overrides.SerializeAsString());

  ReferenceCounted<iris::Material> material = MakeMatteMaterial(
      texture_manager.AllocateReflectorTexture(with_defaults.kd()),
      texture_manager.AllocateFloatTexture(with_defaults.sigma()));

  return MaterialResult{{material, material},
                        MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
