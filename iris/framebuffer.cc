#include "iris/framebuffer.h"

#undef NDEBUG  // Enable assertions at runtime
#include <array>
#include <cassert>
#include <utility>
#include <vector>

#include "iris/color.h"
#include "iris/float.h"

namespace iris {

Framebuffer::Framebuffer(std::pair<size_t, size_t> dimensions)
    : pixels_(dimensions.first,
              std::vector<std::pair<std::array<visual_t, 3>, Color::Space>>(
                  dimensions.second, {{0.0, 0.0, 0.0}, Color::CIE_XYZ})) {}

Color Framebuffer::Get(size_t y, size_t x) const {
  assert(y < pixels_.size());
  assert(x < pixels_.front().size());
  const auto& [components, color_space] = pixels_[y][x];
  return Color(components[0], components[1], components[2], color_space);
}

void Framebuffer::Set(size_t y, size_t x, const Color& color) {
  assert(y < pixels_.size());
  assert(x < pixels_.front().size());
  pixels_[y][x] = std::make_pair(
      std::array<visual_t, 3>({color.x, color.y, color.z}), color.space);
}

std::pair<size_t, size_t> Framebuffer::Size() const {
  return std::make_pair(pixels_.size(), pixels_.front().size());
}

}  // namespace iris
