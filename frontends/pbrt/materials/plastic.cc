#include "frontends/pbrt/materials/plastic.h"

#include <algorithm>
#include <utility>

#include "absl/flags/flag.h"
#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/materials/plastic.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/plastic_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

ABSL_FLAG(bool, reverse_plastic_eta, true,
          "If true, the eta of the faces of a plastic material are reversed. "
          "This replicates a bug that exists in pbrt-v3.");

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakePlasticMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

constexpr visual kDefaultEtaFront = 1.0;
constexpr visual kDefaultEtaBack = 1.5;

MaterialResult MakePlastic(const Material::Plastic& plastic,
                           const Shape::MaterialOverrides& overrides,
                           TextureManager& texture_manager) {
  Material::Plastic with_defaults = Defaults().materials().plastic();
  with_defaults.MergeFrom(plastic);
  with_defaults.MergeFromString(overrides.SerializeAsString());

  visual eta_front = kDefaultEtaFront;
  visual eta_back = kDefaultEtaBack;
  if (absl::GetFlag(FLAGS_reverse_plastic_eta)) {
    std::swap(eta_front, eta_back);
  }

  return MaterialResult{
      {MakePlasticMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.kd()),
           texture_manager.AllocateReflectorTexture(with_defaults.ks()),
           texture_manager.AllocateFloatTexture(eta_front),
           texture_manager.AllocateFloatTexture(eta_back),
           texture_manager.AllocateFloatTexture(with_defaults.roughness()),
           with_defaults.remaproughness()),
       MakePlasticMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.kd()),
           texture_manager.AllocateReflectorTexture(with_defaults.ks()),
           texture_manager.AllocateFloatTexture(eta_back),
           texture_manager.AllocateFloatTexture(eta_front),
           texture_manager.AllocateFloatTexture(with_defaults.roughness()),
           with_defaults.remaproughness())},
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
