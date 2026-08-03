#include "frontends/pbrt/materials/disney.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/disney_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/pbrt.pb.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeDisneyMaterial;
using ::pbrt_proto::DisneyMaterial;
using ::pbrt_proto::v3::Shape;

MaterialResult MakeDisney(const DisneyMaterial& disney,
                          const Shape::MaterialOverrides& overrides,
                          TextureManager& texture_manager) {
  DisneyMaterial with_defaults = Defaults().materials().disney();
  with_defaults.MergeFrom(disney);
  if (!with_defaults.MergeFromString(overrides.SerializeAsString())) {
    std::cerr << "ERROR: Malformed material overrides" << std::endl;
    exit(EXIT_FAILURE);
  }

  return MaterialResult{
      {MakeDisneyMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.color()),
           texture_manager.AllocateFloatTexture(with_defaults.metallic()),
           texture_manager.AllocateFloatTexture(1.0),
           texture_manager.AllocateFloatTexture(with_defaults.eta()),
           texture_manager.AllocateFloatTexture(with_defaults.roughness()),
           texture_manager.AllocateFloatTexture(with_defaults.speculartint()),
           texture_manager.AllocateFloatTexture(with_defaults.anisotropic()),
           texture_manager.AllocateFloatTexture(with_defaults.sheen()),
           texture_manager.AllocateFloatTexture(with_defaults.sheentint()),
           texture_manager.AllocateFloatTexture(with_defaults.clearcoat()),
           texture_manager.AllocateFloatTexture(with_defaults.clearcoatgloss()),
           texture_manager.AllocateFloatTexture(with_defaults.spectrans()),
           texture_manager.AllocateFloatTexture(with_defaults.flatness()),
           texture_manager.AllocateFloatTexture(with_defaults.difftrans()),
           texture_manager.AllocateReflectorTexture(
               with_defaults.scatterdistance()),
           with_defaults.thin()),
       MakeDisneyMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.color()),
           texture_manager.AllocateFloatTexture(with_defaults.metallic()),
           texture_manager.AllocateFloatTexture(with_defaults.eta()),
           texture_manager.AllocateFloatTexture(1.0),
           texture_manager.AllocateFloatTexture(with_defaults.roughness()),
           texture_manager.AllocateFloatTexture(with_defaults.speculartint()),
           texture_manager.AllocateFloatTexture(with_defaults.anisotropic()),
           texture_manager.AllocateFloatTexture(with_defaults.sheen()),
           texture_manager.AllocateFloatTexture(with_defaults.sheentint()),
           texture_manager.AllocateFloatTexture(with_defaults.clearcoat()),
           texture_manager.AllocateFloatTexture(with_defaults.clearcoatgloss()),
           texture_manager.AllocateFloatTexture(with_defaults.spectrans()),
           texture_manager.AllocateFloatTexture(with_defaults.flatness()),
           texture_manager.AllocateFloatTexture(with_defaults.difftrans()),
           texture_manager.AllocateReflectorTexture(
               with_defaults.scatterdistance()),
           with_defaults.thin())},
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
