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

  if (overrides.bumpmap().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_bumpmap()->MergeFrom(overrides.bumpmap());
  }

  if (overrides.kd().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_kd()->MergeFrom(overrides.kd());
  }

  if (overrides.ks().spectrum_texture_parameter_type_case() !=
      SpectrumTextureParameter::SPECTRUM_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_ks()->MergeFrom(overrides.ks());
  }

  if (overrides.roughness().float_texture_parameter_type_case() !=
      FloatTextureParameter::FLOAT_TEXTURE_PARAMETER_TYPE_NOT_SET) {
    with_defaults.mutable_roughness()->MergeFrom(overrides.roughness());
  }

  if (overrides.has_remaproughness()) {
    with_defaults.set_remaproughness(overrides.remaproughness());
  }

  FloatTextureParameter eta_front;
  eta_front.set_float_value(kDefaultEtaFront);
  FloatTextureParameter eta_back;
  eta_back.set_float_value(kDefaultEtaBack);
  if (absl::GetFlag(FLAGS_reverse_plastic_eta)) {
    std::swap(eta_front, eta_back);
  }

  return MaterialResult{
      MakePlasticMaterial(
          texture_manager.AllocateReflectorTexture(with_defaults.kd()),
          texture_manager.AllocateReflectorTexture(with_defaults.ks()),
          texture_manager.AllocateFloatTexture(eta_front),
          texture_manager.AllocateFloatTexture(eta_back),
          texture_manager.AllocateFloatTexture(with_defaults.roughness()),
          with_defaults.remaproughness()),
      MakeBumpMap(with_defaults.bumpmap(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
