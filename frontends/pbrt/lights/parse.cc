#include "frontends/pbrt/lights/parse.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/lights/distant.h"
#include "frontends/pbrt/lights/infinite.h"
#include "frontends/pbrt/quoted_string.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

static const std::unordered_map<
    std::string_view, const std::unique_ptr<const ObjectBuilder<
                          std::variant<ReferenceCounted<Light>,
                                       ReferenceCounted<EnvironmentalLight>>,
                          SpectrumManager&, const Matrix&>>&>
    g_lights = {{"distant", g_distant_builder},
                {"infinite", g_infinite_builder}};

}  // namespace

const ObjectBuilder<
    std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>,
    SpectrumManager&, const Matrix&>&
Parse(Tokenizer& tokenizer) {
  std::optional<std::string_view> type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: LightSource"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::optional<std::string_view> unquoted = Unquote(*type);
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

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris