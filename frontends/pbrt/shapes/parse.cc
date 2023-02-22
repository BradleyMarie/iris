#include "frontends/pbrt/shapes/parse.h"

#include <unordered_map>

#include "frontends/pbrt/quoted_string.h"
#include "frontends/pbrt/shapes/trianglemesh.h"

namespace iris::pbrt_frontend::shapes {
namespace {

static const std::unordered_map<
    std::string_view,
    const std::unique_ptr<const ObjectBuilder<
        std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
        const ReferenceCounted<iris::Material>&,
        const ReferenceCounted<iris::NormalMap>&,
        const ReferenceCounted<EmissiveMaterial>&,
        const ReferenceCounted<EmissiveMaterial>&, const Matrix&>>&>
    g_shapes = {{"trianglemesh", g_trianglemesh_builder}};

}  // namespace

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> Parse(
    Tokenizer& tokenizer, SpectrumManager& spectrum_manager,
    TextureManager& texture_manager,
    const std::shared_ptr<ObjectBuilder<
        std::pair<ReferenceCounted<Material>, ReferenceCounted<NormalMap>>,
        TextureManager&>>& material_builder,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const Matrix& model_to_world) {
  auto type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: Shape" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*type);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to Shape must be a string" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto iter = g_shapes.find(*unquoted);
  if (iter == g_shapes.end()) {
    std::cerr << "ERROR: Unsupported type for directive Shape: " << *unquoted
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::unordered_set<std::string_view> parameters_parsed;
  std::unordered_map<std::string_view, Parameter> material_parameters;
  std::unordered_map<std::string_view, Parameter> shape_parameters;

  ParameterList parameter_list;
  while (parameter_list.ParseFrom(tokenizer)) {
    auto material_parameter = material_builder->Parse(
        parameter_list, tokenizer.SearchRoot(), spectrum_manager,
        texture_manager, parameters_parsed,
        /*must_succeed=*/false);
    if (material_parameter) {
      auto name = *parameters_parsed.find(parameter_list.GetName());
      material_parameters[name] = std::move(*material_parameter);
      continue;
    }

    auto shape_parameter =
        iter->second
            ->Parse(parameter_list, tokenizer.SearchRoot(), spectrum_manager,
                    texture_manager, parameters_parsed)
            .value();
    auto name = *parameters_parsed.find(parameter_list.GetName());
    shape_parameters[name] = std::move(shape_parameter);
  }

  auto material = material_builder->Build(material_parameters, texture_manager);

  return iter->second->Build(shape_parameters, material.first, material.second,
                             front_emissive_material, back_emissive_material,
                             model_to_world);
}

}  // namespace iris::pbrt_frontend::shapes