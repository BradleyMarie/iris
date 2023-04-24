#ifndef _IRIS_TEXTURE_COORDINATES_
#define _IRIS_TEXTURE_COORDINATES_

#include <array>
#include <optional>

#include "iris/float.h"
#include "iris/vector.h"

namespace iris {

struct TextureCoordinates final {
  struct Derivatives {
    geometric_t du_dx;
    geometric_t du_dy;
    geometric_t dv_dx;
    geometric_t dv_dy;
    Vector dp_du;
    Vector dp_dv;
  };

  const std::array<geometric_t, 2> uv;
  const std::optional<Derivatives> derivatives;

  TextureCoordinates Scale(geometric u_scale, geometric v_scale) const {
    assert(std::isfinite(u_scale) && u_scale != 0.0);
    assert(std::isfinite(v_scale) && v_scale != 0.0);

    std::array<geometric_t, 2> new_uv;
    new_uv[0] = uv[0] * u_scale;
    new_uv[1] = uv[1] * v_scale;

    if (!derivatives) {
      return {new_uv};
    }

    TextureCoordinates::Derivatives new_derivatives{
        derivatives->du_dx * u_scale, derivatives->du_dy * u_scale,
        derivatives->dv_dx * v_scale, derivatives->dv_dy * v_scale,
        derivatives->dp_du / u_scale, derivatives->dp_dv / v_scale};

    return {new_uv, new_derivatives};
  }
};

}  // namespace iris

#endif  // _IRIS_TEXTURE_COORDINATES_