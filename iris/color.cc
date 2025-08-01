#include "iris/color.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace iris {
namespace {

static const visual linear_srgb_to_xyz[3][3] = {
    {0.4124564, 0.3575761, 0.1804375},
    {0.2126729, 0.7151522, 0.0721750},
    {0.0193339, 0.1191920, 0.9503041}};
static const visual xyz_to_linear_srgb[3][3] = {
    {3.2404542, -1.5371385, -0.4985314},
    {-0.9692660, 1.8760108, 0.0415560},
    {0.0556434, -0.2040259, 1.0572252}};

}  // namespace

#ifndef NDEBUG

Color::Color(visual_t c0, visual_t c1, visual_t c2, Space space) noexcept
    : x(c0), y(c1), z(c2), space(space) {
  assert(std::isfinite(x) && x >= 0.0);
  assert(std::isfinite(y) && y >= 0.0);
  assert(std::isfinite(z) && z >= 0.0);
  assert(space >= CIE_XYZ);
  assert(space <= LINEAR_SRGB);
}

#endif  // NDEBUG

Color Color::ConvertTo(Space target) const {
  assert(target >= CIE_XYZ);
  assert(target <= LINEAR_SRGB);

  if (space == target) {
    return *this;
  }

  const visual(*transformation)[3] =
      (target == CIE_XYZ) ? linear_srgb_to_xyz : xyz_to_linear_srgb;

  visual_t u = x * transformation[0][0] + y * transformation[0][1] +
               z * transformation[0][2];
  u = std::max(static_cast<visual_t>(0.0), u);

  visual_t v = x * transformation[1][0] + y * transformation[1][1] +
               z * transformation[1][2];
  v = std::max(static_cast<visual_t>(0.0), v);

  visual_t w = x * transformation[2][0] + y * transformation[2][1] +
               z * transformation[2][2];
  w = std::max(static_cast<visual_t>(0.0), w);

  return Color(u, v, w, target);
}

visual_t Color::Luma() const {
  if (space == CIE_XYZ) {
    return y;
  }

  const visual(*transformation)[3] = linear_srgb_to_xyz;

  return std::max(static_cast<visual_t>(0.0), r * transformation[1][0] +
                                                  g * transformation[1][1] +
                                                  b * transformation[1][2]);
}

}  // namespace iris
