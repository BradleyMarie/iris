#include "frontends/pbrt/materials/parse.h"

#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "frontends/pbrt/materials/glass.h"
#include "frontends/pbrt/materials/matte.h"
#include "frontends/pbrt/materials/mirror.h"
#include "frontends/pbrt/materials/plastic.h"
#include "frontends/pbrt/materials/translucent.h"
#include "frontends/pbrt/materials/uber.h"
#include "frontends/pbrt/quoted_string.h"

namespace iris::pbrt_frontend::materials {
namespace {

static const std::unordered_map<
    std::string_view,
    const std::unique_ptr<const ObjectBuilder<
        std::shared_ptr<ObjectBuilder<
            std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                       ReferenceCounted<NormalMap>,
                       ReferenceCounted<NormalMap>>,
            TextureManager&>>,
        TextureManager&>>&>
    g_materials = {{"glass", g_glass_builder},
                   {"matte", g_matte_builder},
                   {"mirror", g_mirror_builder},
                   {"plastic", g_plastic_builder},
                   {"translucent", g_translucent_builder},
                   {"uber", g_uber_builder}};

}  // namespace

const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>&
Parse(Tokenizer& tokenizer) {
  auto type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: Material"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*type);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to Material must be a string" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto iter = g_materials.find(*unquoted);
  if (iter == g_materials.end()) {
    std::cerr << "ERROR: Unsupported type for directive Material: " << *unquoted
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return *iter->second;
}

void ParseNamed(Tokenizer& tokenizer, const std::filesystem::path& search_root,
                MaterialManager& material_manager,
                SpectrumManager& spectrum_manager,
                TextureManager& texture_manager) {
  auto type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: MakeNamedMaterial"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*type);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to MakeNamedMaterial must be a string"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string name(unquoted->begin(), unquoted->end());

  std::unordered_map<std::string_view, std::unique_ptr<ParameterList>>
      parameter_lists;

  auto parameter_list = std::make_unique<ParameterList>();
  while (parameter_list->ParseFrom(tokenizer)) {
    auto insertion_result = parameter_lists.emplace(parameter_list->GetName(),
                                                    std::move(parameter_list));

    if (!insertion_result.second) {
      std::cerr << "ERROR: A parameter was specified twice: "
                << insertion_result.first->first << std::endl;
      exit(EXIT_FAILURE);
    }

    parameter_list = std::make_unique<ParameterList>();
  }

  auto type_iter = parameter_lists.find("type");
  if (type_iter == parameter_lists.end()) {
    std::cerr << "ERROR: Missing required parameter to MakeNamedMaterial: type"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  Parameter type_parameter;
  type_parameter.LoadFrom(*type_iter->second, search_root, Parameter::STRING,
                          spectrum_manager, texture_manager);

  auto material_builder =
      g_materials.find(type_parameter.GetStringValues(1).front());
  if (material_builder == g_materials.end()) {
    std::cerr << "ERROR: Unsupported type for directive MakeNamedMaterial: "
              << type_parameter.GetStringValues(1).front() << std::endl;
    exit(EXIT_FAILURE);
  }

  parameter_lists.erase(type_iter);

  std::unordered_set<std::string_view> parameters_parsed;
  std::unordered_map<std::string_view, Parameter> parameters;
  for (const auto& [parameter_name, parameter_list] : parameter_lists) {
    auto parameter = material_builder->second
                         ->Parse(*parameter_list, search_root, spectrum_manager,
                                 texture_manager, parameters_parsed)
                         .value();
    parameters[parameter_name] = std::move(parameter);
  }

  auto result = material_builder->second->Build(parameters, texture_manager);
  material_manager.Put(name, result);
}

const ObjectBuilder<
    std::shared_ptr<ObjectBuilder<
        std::tuple<ReferenceCounted<Material>, ReferenceCounted<Material>,
                   ReferenceCounted<NormalMap>, ReferenceCounted<NormalMap>>,
        TextureManager&>>,
    TextureManager&>&
Default() {
  return *g_matte_builder;
}

}  // namespace iris::pbrt_frontend::materials