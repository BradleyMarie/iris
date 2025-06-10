#ifndef _IRIS_FRAMEBUFFER_
#define _IRIS_FRAMEBUFFER_

#include <array>
#include <utility>
#include <vector>

#include "iris/color.h"
#include "iris/float.h"

namespace iris {

class Framebuffer final {
 public:
  Framebuffer(std::pair<size_t, size_t> dimensions);

  Color Get(size_t y, size_t x) const;
  void Set(size_t y, size_t x, const Color& color);

  std::pair<size_t, size_t> Size() const;

 private:
  std::vector<std::vector<std::pair<std::array<visual_t, 3>, Color::Space>>>
      pixels_;
};

}  // namespace iris

#endif  // _IRIS_FRAMEBUFFER_
