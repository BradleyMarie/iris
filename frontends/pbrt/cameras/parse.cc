#include "frontends/pbrt/cameras/parse.h"

#include <cstdlib>
#include <iostream>

#include "frontends/pbrt/cameras/orthographic.h"
#include "frontends/pbrt/cameras/perspective.h"
#include "frontends/pbrt/quoted_string.h"

namespace iris::pbrt_frontend::cameras {
namespace {

static const std::unordered_map<std::string_view,
                                const std::unique_ptr<const ObjectBuilder<
                                    std::function<std::unique_ptr<Camera>(
                                        const std::pair<size_t, size_t>&)>,
                                    const MatrixManager::Transformation&>>&>
    g_cameras = {{"orthographic", g_orthographic_builder},
                 {"perspective", g_perspective_builder}};

}  // namespace

const ObjectBuilder<
    std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>,
    const MatrixManager::Transformation&>&
Parse(Tokenizer& tokenizer) {
  auto type = tokenizer.Next();
  if (!type) {
    std::cerr << "ERROR: Too few parameters to directive: Camera" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto unquoted = Unquote(*type);
  if (!unquoted) {
    std::cerr << "ERROR: Parameter to Camera must be a string" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto iter = g_cameras.find(*unquoted);
  if (iter == g_cameras.end()) {
    std::cerr << "ERROR: Unsupported type for directive Camera: " << *unquoted
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return *iter->second;
}

const ObjectBuilder<
    std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>,
    const MatrixManager::Transformation&>&
Default() {
  return *g_perspective_builder;
}

}  // namespace iris::pbrt_frontend::cameras