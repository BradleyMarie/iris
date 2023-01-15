#include "frontends/pbrt/shapes/parse.h"

#include <unordered_map>

#include "frontends/pbrt/quoted_string.h"

namespace iris::pbrt_frontend::shapes {
namespace {

static const std::unordered_map<
    std::string_view,
    const ObjectBuilder<
        std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
        TextureManager&,
        const std::shared_ptr<ObjectBuilder<
            iris::ReferenceCounted<iris::Material>, TextureManager&>>&,
        const ReferenceCounted<EmissiveMaterial>&,
        const ReferenceCounted<EmissiveMaterial>&, const Matrix&>*>
    g_shapes = {};

}  // namespace

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> Parse(
    Tokenizer& tokenizer, SpectrumManager& spectrum_manager,
    TextureManager& texture_manager,
    const std::shared_ptr<ObjectBuilder<iris::ReferenceCounted<iris::Material>,
                                        TextureManager&>>& material,
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

  return {std::vector<ReferenceCounted<Geometry>>(), Matrix::Identity()};
}

}  // namespace iris::pbrt_frontend::shapes