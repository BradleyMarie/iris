#include "frontends/pbrt/textures/parse.h"

#include <unordered_map>

#include "frontends/pbrt/quoted_string.h"

namespace iris::pbrt_frontend::textures {
namespace {

static const std::unordered_map<
    std::string_view,
    const std::unique_ptr<const ObjectBuilder<void, TextureManager&>>&>
    g_float_textures = {};

static const std::unordered_map<
    std::string_view,
    const std::unique_ptr<const ObjectBuilder<void, TextureManager&>>&>
    g_spectral_textures = {};

}  // namespace

const ObjectBuilder<void, TextureManager&>& Parse(Tokenizer& tokenizer) {
  auto name = tokenizer.Next();
  if (!name) {
    std::cerr << "ERROR: Too few parameters to directive: Texture" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted_name = Unquote(*name);
  if (!unquoted_name) {
    std::cerr << "ERROR: Name of Texture must be a string" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto kind = tokenizer.Next();
  if (!kind) {
    std::cerr << "ERROR: Too few parameters to directive: Texture" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted_kind = Unquote(*kind);
  if (!unquoted_kind) {
    std::cerr << "ERROR: Value type of Texture must be a string"
              << *unquoted_kind << std::endl;
    exit(EXIT_FAILURE);
  }

  bool float_texture;
  if (*unquoted_kind == "float") {
    float_texture = true;
  } else if (*unquoted_kind == "spectrum" || *unquoted_kind == "color") {
    float_texture = false;
  } else {
    std::cerr << "ERROR: Unsupported value type for directive Texture: "
              << *unquoted_kind << std::endl;
    exit(EXIT_FAILURE);
  }

  auto type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: Texture" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted_type = Unquote(*type);
  if (!unquoted_type) {
    std::cerr << "ERROR: Type of Texture must be a string" << std::endl;
    exit(EXIT_FAILURE);
  }

  const ObjectBuilder<void, TextureManager&>* result;
  if (float_texture) {
    auto iter = g_float_textures.find(*unquoted_type);
    if (iter == g_float_textures.end()) {
      std::cerr << "ERROR: Unsupported type for directive Texture: "
                << *unquoted_type << std::endl;
      exit(EXIT_FAILURE);
    }

    result = iter->second.get();
  } else {
    auto iter = g_spectral_textures.find(*unquoted_type);
    if (iter == g_spectral_textures.end()) {
      std::cerr << "ERROR: Unsupported type for directive Texture: "
                << *unquoted_type << std::endl;
      exit(EXIT_FAILURE);
    }

    result = iter->second.get();
  }

  return *result;
}

}  // namespace iris::pbrt_frontend::textures