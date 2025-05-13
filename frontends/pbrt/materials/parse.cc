#include "frontends/pbrt/materials/parse.h"

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/glass.h"
#include "frontends/pbrt/materials/matte.h"
#include "frontends/pbrt/materials/metal.h"
#include "frontends/pbrt/materials/mirror.h"
#include "frontends/pbrt/materials/mix.h"
#include "frontends/pbrt/materials/plastic.h"
#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/materials/substrate.h"
#include "frontends/pbrt/materials/translucent.h"
#include "frontends/pbrt/materials/uber.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::MakeNamedMaterial;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;

MaterialResult ParseMaterial(const Material& material,
                             const Shape::MaterialOverrides& overrides,
                             const MaterialManager& material_manager,
                             TextureManager& texture_manager,
                             SpectrumManager& spectrum_manager) {
  MaterialResult result;
  switch (material.material_type_case()) {
    case Material::kDisney:
      break;
    case Material::kFourier:
      break;
    case Material::kGlass:
      result =
          materials::MakeGlass(material.glass(), overrides, texture_manager);
      break;
    case Material::kHair:
      break;
    case Material::kKdsubsurface:
      break;
    case Material::kMatte:
      result =
          materials::MakeMatte(material.matte(), overrides, texture_manager);
      break;
    case Material::kMetal:
      result = materials::MakeMetal(material.metal(), overrides,
                                    texture_manager, spectrum_manager);
      break;
    case Material::kMirror:
      result =
          materials::MakeMirror(material.mirror(), overrides, texture_manager);
      break;
    case Material::kMix:
      result = materials::MakeMix(material.mix(), overrides, material_manager,
                                  texture_manager);
      break;
    case Material::kPlastic:
      result = materials::MakePlastic(material.plastic(), overrides,
                                      texture_manager);
      break;
    case Material::kSubstrate:
      result = materials::MakeSubstrate(material.substrate(), overrides,
                                        texture_manager);
      break;
    case Material::kSubsurface:
      break;
    case Material::kTranslucent:
      result = materials::MakeTranslucent(material.translucent(), overrides,
                                          texture_manager);
      break;
    case Material::kUber:
      result = materials::MakeUber(material.uber(), overrides, texture_manager);
      break;
    case Material::MATERIAL_TYPE_NOT_SET:
      break;
  }
  return result;
}

MaterialResult ParseMakeNamedMaterial(const MakeNamedMaterial& named_material,
                                      MaterialManager& material_manager,
                                      TextureManager& texture_manager,
                                      SpectrumManager& spectrum_manager) {
  MaterialResult result = ParseMaterial(
      named_material.material(), Shape::MaterialOverrides::default_instance(),
      material_manager, texture_manager, spectrum_manager);
  material_manager.Put(named_material.name(),
                       {named_material.material(), result});
  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
