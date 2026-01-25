#include "frontends/pbrt/materials/parse.h"

#include "frontends/pbrt/material_manager.h"
#include "frontends/pbrt/materials/disney.h"
#include "frontends/pbrt/materials/fourier.h"
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
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::MakeNamedMaterial;
using ::pbrt_proto::v3::Material;
using ::pbrt_proto::v3::Shape;

MaterialResult ParseMaterial(const Material& material,
                             const Shape::MaterialOverrides& overrides,
                             const std::filesystem::path& search_root,
                             const MaterialManager& material_manager,
                             TextureManager& texture_manager,
                             SpectrumManager& spectrum_manager) {
  MaterialResult result;
  switch (material.material_type_case()) {
    case Material::kDisney:
      result =
          materials::MakeDisney(material.disney(), overrides, texture_manager);
      break;
    case Material::kFourier:
      result =
          materials::MakeFourier(material.fourier(), overrides, search_root,
                                 texture_manager, spectrum_manager);
      break;
    case Material::kGlass:
      result =
          materials::MakeGlass(material.glass(), overrides, texture_manager);
      break;
    case Material::kHair:
      std::cerr << "ERROR: Unsupported Material type: hair" << std::endl;
      exit(EXIT_FAILURE);
      break;
    case Material::kKdsubsurface:
      std::cerr << "ERROR: Unsupported Material type: kdsubsurface"
                << std::endl;
      exit(EXIT_FAILURE);
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
      std::cerr << "ERROR: Unsupported Material type: subsurface" << std::endl;
      exit(EXIT_FAILURE);
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
                                      const std::filesystem::path& search_root,
                                      MaterialManager& material_manager,
                                      TextureManager& texture_manager,
                                      SpectrumManager& spectrum_manager) {
  MaterialResult result = ParseMaterial(
      named_material.material(), Shape::MaterialOverrides::default_instance(),
      search_root, material_manager, texture_manager, spectrum_manager);
  material_manager.Put(named_material.name(),
                       {named_material.material(), result});
  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
