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
  with_defaults.MergeFromString(overrides.SerializeAsString());

  auto eta = texture_manager.AllocateFloatTexture(with_defaults.eta());

  return MaterialResult{
      MakeUberMaterial(
          texture_manager.AllocateReflectorTexture(with_defaults.kr()),
          texture_manager.AllocateReflectorTexture(with_defaults.kt()),
          texture_manager.AllocateReflectorTexture(with_defaults.kd()),
          texture_manager.AllocateReflectorTexture(with_defaults.ks()),
          texture_manager.AllocateFloatTexture(with_defaults.opacity()),
          texture_manager.AllocateFloatTexture(kDefaultEtaIncident),
          texture_manager.AllocateFloatTexture(with_defaults.eta()),
          texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
          texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
          with_defaults.remaproughness()),
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
