#ifndef _FRONTENDS_PBRT_PARSER_
#define _FRONTENDS_PBRT_PARSER_

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>

#include "frontends/pbrt/directives.h"
#include "frontends/pbrt/renderable.h"
#include "iris/float.h"
#include "iris/renderer.h"

namespace iris {
namespace pbrt_frontend {

struct ParsingResult {
  Renderable renderable;
  Renderer::SkipPixelFn skip_pixel_callback;
  std::filesystem::path output_filename;
  std::function<void(Framebuffer&, std::ofstream&)> output_write_function;
  visual_t max_sample_luminance;
};

struct Options {
  bool always_reflective = false;
};

std::unique_ptr<ParsingResult> ParseScene(
    Directives& directives, const Options& options,
    const std::filesystem::path& search_root);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_PARSER_
