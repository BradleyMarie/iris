#include "frontends/pbrt/materials/glass.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/glass_material.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/pbrt.pb.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

using ::iris::materials::MakeGlassMaterial;
using ::iris::textures::FloatTexture;
using ::pbrt_proto::DielectricMaterial;
using ::pbrt_proto::v3::Shape;

constexpr visual kDefaultEtaFront = 1.0;

MaterialResult MakeGlass(const DielectricMaterial& glass,
                         const Shape::MaterialOverrides& overrides,
                         TextureManager& texture_manager) {
  DielectricMaterial with_defaults = Defaults().materials().glass();
  with_defaults.MergeFrom(glass);
  if (!with_defaults.MergeFromString(overrides.SerializeAsString())) {
    std::cerr << "ERROR: Malformed material overrides" << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<FloatTexture> eta;
  switch (glass.eta_type_case()) {
    case DielectricMaterial::kEta:
      switch (with_defaults.eta().spectrum_type_case()) {
        case pbrt_proto::Spectrum::kConstantSpectrum:
          eta = texture_manager.AllocateFloatTexture(
              with_defaults.eta().constant_spectrum());
          break;
        case pbrt_proto::Spectrum::SPECTRUM_TYPE_NOT_SET:
          break;
        default:
          std::cerr << "ERROR: Unsupported eta type" << std::endl;
          exit(EXIT_FAILURE);
          break;
      }
      break;
    case DielectricMaterial::kEtaFloatTextureName:
      eta = texture_manager.GetFloatTexture(
          with_defaults.eta_float_texture_name());
      break;
    case DielectricMaterial::ETA_TYPE_NOT_SET:
      break;
  }

  return MaterialResult{
      {MakeGlassMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.kr()),
           texture_manager.AllocateReflectorTexture(with_defaults.kt()),
           texture_manager.AllocateFloatTexture(kDefaultEtaFront), eta,
           texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
           texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
           with_defaults.remaproughness()),
       MakeGlassMaterial(
           texture_manager.AllocateReflectorTexture(with_defaults.kr()),
           texture_manager.AllocateReflectorTexture(with_defaults.kt()), eta,
           texture_manager.AllocateFloatTexture(kDefaultEtaFront),
           texture_manager.AllocateFloatTexture(with_defaults.uroughness()),
           texture_manager.AllocateFloatTexture(with_defaults.vroughness()),
           with_defaults.remaproughness())},
      MakeBumpMap(with_defaults.displacement(), texture_manager)};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
