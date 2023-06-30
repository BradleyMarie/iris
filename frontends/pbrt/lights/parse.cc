#include "frontends/pbrt/lights/parse.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/lights/infinite.h"
#include "frontends/pbrt/quoted_string.h"

namespace iris::pbrt_frontend::lights {
namespace {

static const std::unordered_map<
    std::string_view, const std::unique_ptr<const ObjectBuilder<
                          std::variant<ReferenceCounted<Light>,
                                       ReferenceCounted<EnvironmentalLight>>,
                          SpectrumManager&, const Matrix&>>&>
    g_lights = {{"infinite", g_infinite_builder}};

}  // namespace

const ObjectBuilder<
    std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>,
    SpectrumManager&, const Matrix&>&
Parse(Tokenizer& tokenizer) {
  auto type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: LightSource"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*type);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to LightSource must be a string"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto iter = g_lights.find(*unquoted);
  if (iter == g_lights.end()) {
    std::cerr << "ERROR: Unsupported type for directive LightSource: "
              << *unquoted << std::endl;
    exit(EXIT_FAILURE);
  }

  return *iter->second;
}

}  // namespace iris::pbrt_frontend::lights