#include "frontends/pbrt/samplers/parse.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/quoted_string.h"
#include "frontends/pbrt/samplers/halton.h"
#include "frontends/pbrt/samplers/random.h"
#include "frontends/pbrt/samplers/sobol.h"
#include "frontends/pbrt/samplers/stratified.h"

namespace iris::pbrt_frontend::samplers {
namespace {

static const std::unordered_map<std::string_view,
                                const std::unique_ptr<const ObjectBuilder<
                                    std::unique_ptr<iris::ImageSampler>>>&>
    g_samplers = {{"halton", g_halton_builder},
                  {"random", g_random_builder},
                  {"sobol", g_sobol_builder},
                  {"stratified", g_stratified_builder}};

}  // namespace

const ObjectBuilder<std::unique_ptr<iris::ImageSampler>>& Parse(
    Tokenizer& tokenizer) {
  auto type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: Sampler" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*type);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to Sampler must be a string" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto iter = g_samplers.find(*unquoted);
  if (iter == g_samplers.end()) {
    std::cerr << "ERROR: Unsupported type for directive Sampler: " << *unquoted
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return *iter->second;
}

const ObjectBuilder<std::unique_ptr<iris::ImageSampler>>& Default() {
  return *g_halton_builder;
}

}  // namespace iris::pbrt_frontend::samplers