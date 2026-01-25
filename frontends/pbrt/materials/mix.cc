#include "frontends/pbrt/materials/mix.h"

#include "frontends/pbrt/defaults.h"
#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/bumpmap.h"
#include "frontends/pbrt/materials/matte.h"
#include "frontends/pbrt/texture_manager.h"
#include "iris/material.h"
#include "iris/materials/mix_material.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {
namespace {

using ::iris::materials::MakeMixMaterial;
using ::pbrt_proto::v3::FloatTextureParameter;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;
using ::pbrt_proto::v3::SpectrumTextureParameter;

MaterialResult MakeMixedMaterial(
    const std::string& named_material, const pbrt_proto::v3::Material::Mix& mix,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    const MaterialManager& material_manager, TextureManager& texture_manager) {
  if (const auto* existing_material = material_manager.Get(named_material);
      existing_material) {
    return existing_material->second;
  }

  Material::Matte matte;
  matte.MergeFromString(mix.SerializeAsString());

  return MakeMatte(matte, overrides, texture_manager);
}

}  // namespace

MaterialResult MakeMix(
    const pbrt_proto::v3::Material::Mix& mix,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    const MaterialManager& material_manager, TextureManager& texture_manager) {
  Material::Mix with_defaults = Defaults().materials().mix();
  with_defaults.MergeFrom(mix);
  with_defaults.MergeFromString(overrides.SerializeAsString());

  MaterialResult material1 =
      MakeMixedMaterial(with_defaults.namedmaterial1(), mix, overrides,
                        material_manager, texture_manager);
  MaterialResult material2 =
      MakeMixedMaterial(with_defaults.namedmaterial2(), mix, overrides,
                        material_manager, texture_manager);

  return MaterialResult{
      {MakeMixMaterial(
           material1.materials[0], material2.materials[0],
           texture_manager.AllocateReflectorTexture(with_defaults.amount())),
       MakeMixMaterial(
           material1.materials[1], material2.materials[1],
           texture_manager.AllocateReflectorTexture(with_defaults.amount()))},
      {
          material1.bumpmaps[0] ? material1.bumpmaps[0] : material2.bumpmaps[0],
          material1.bumpmaps[1] ? material1.bumpmaps[1] : material2.bumpmaps[1],
      }};
}

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris
