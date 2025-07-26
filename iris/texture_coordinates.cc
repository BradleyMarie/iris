#include "iris/texture_coordinates.h"

#include <array>
#include <cassert>

#include "iris/float.h"

namespace iris {

TextureCoordinates TextureCoordinates::Scale(geometric u_scale,
                                             geometric v_scale,
                                             geometric u_offset,
                                             geometric v_offset) const {
  assert(std::isfinite(u_scale) && u_scale != 0.0);
  assert(std::isfinite(v_scale) && v_scale != 0.0);
  assert(std::isfinite(u_offset));
  assert(std::isfinite(v_offset));

  return {p,
          dp_dx,
          dp_dy,
          {uv[0] * u_scale + u_offset, uv[1] * v_scale + v_offset},
          du_dx * u_scale,
          du_dy * u_scale,
          dv_dx * v_scale,
          dv_dy * v_scale};
}

}  // namespace iris
