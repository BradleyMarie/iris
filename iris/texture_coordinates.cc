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

  std::array<geometric_t, 2> new_uv;
  new_uv[0] = uv[0] * u_scale + u_offset;
  new_uv[1] = uv[1] * v_scale + v_offset;

  if (!differentials) {
    return {new_uv};
  }

  TextureCoordinates::Differentials new_differentials{
      differentials->du_dx * u_scale, differentials->du_dy * u_scale,
      differentials->dv_dx * v_scale, differentials->dv_dy * v_scale};

  return {new_uv, new_differentials};
}

}  // namespace iris
