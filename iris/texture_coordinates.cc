#include "iris/texture_coordinates.h"

#include <array>
#include <cassert>

#include "iris/float.h"

namespace iris {

TextureCoordinates TextureCoordinates::Offset(const Vector& p_offset,
                                              geometric_t u_offset,
                                              geometric_t v_offset) const {
  assert(std::isfinite(u_offset));
  assert(std::isfinite(v_offset));

  return {p + p_offset, dp_dx, dp_dy, {uv[0] + u_offset, uv[1] + v_offset},
          du_dx,        du_dy, dv_dx, dv_dy,
          face_index};
}

TextureCoordinates TextureCoordinates::Scale(geometric u_scale,
                                             geometric v_scale) const {
  assert(std::isfinite(u_scale) && u_scale != 0.0);
  assert(std::isfinite(v_scale) && v_scale != 0.0);

  return {p,
          dp_dx,
          dp_dy,
          {uv[0] * u_scale, uv[1] * v_scale},
          du_dx * u_scale,
          du_dy * u_scale,
          dv_dx * v_scale,
          dv_dy * v_scale,
          face_index};
}

}  // namespace iris
