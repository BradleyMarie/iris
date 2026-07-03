#include "frontends/pbrt/materials/matte.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/matte_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/pbrt.pb.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeMatteMaterial;
using ::pbrt_proto::DiffuseMaterial;
using ::pbrt_proto::v3::Shape;

MaterialResult MakeMatte(const DiffuseMaterial& matte,
                         const Shape::MaterialOverrides& overrides,
                         TextureManager& texture_manager) {
  DiffuseMaterial with_defaults = Defaults().materials().matte();
  with_defaults.MergeFrom(matte);
  if (!with_defaults.MergeFromString(overrides.SerializeAsString())) {
    std::cerr << "ERROR: Malformed material overrides" << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<Material> material = MakeMatteMaterial(
      texture_manager.AllocateReflectorTexture(with_defaults.reflectance()),
      texture_manager.AllocateFloatTexture(with_defaults.sigma()));

  return MaterialResult{
      {material, material},
      MakeBumpMap(with_defaults.displacement(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
