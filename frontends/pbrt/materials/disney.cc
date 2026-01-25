#include "frontends/pbrt/materials/disney.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/materials/disney.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

MaterialResult MakeDisney(const Material::Disney& disney,
                          const Shape::MaterialOverrides& overrides,
                          TextureManager& texture_manager) {
  Material::Disney with_defaults = Defaults().materials().disney();
  with_defaults.MergeFrom(disney);
  with_defaults.MergeFromString(overrides.SerializeAsString());

  std::cerr << "ERROR: Unsupported Material type: disney" << std::endl;
  exit(EXIT_FAILURE);
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
