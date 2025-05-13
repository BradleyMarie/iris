#ifndef _FRONTENDS_PBRT_FILM_RESULT_
#define _FRONTENDS_PBRT_FILM_RESULT_

#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <string>

#include "iris/framebuffer.h"

namespace iris {
namespace pbrt_frontend {

struct FilmResult {
  std::filesystem::path filename;
  std::pair<size_t, size_t> resolution;
  std::function<bool(std::pair<size_t, size_t>, std::pair<size_t, size_t>)>
      skip_pixel_function;
  std::function<void(Framebuffer&, std::ofstream&)> write_function;
  geometric_t diagonal;
  std::optional<visual_t> max_sample_luminance;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_FILM_RESULT_
