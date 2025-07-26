#ifndef _IRIS_TEXTURE_COORDINATES_
#define _IRIS_TEXTURE_COORDINATES_

#include <array>
#include <optional>

#include "iris/float.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {

struct TextureCoordinates final {
  const Point p;
  const Vector dp_dx;
  const Vector dp_dy;

  const geometric_t uv[2];
  const geometric_t du_dx;
  const geometric_t du_dy;
  const geometric_t dv_dx;
  const geometric_t dv_dy;

  TextureCoordinates Scale(geometric u_scale, geometric v_scale,
                           geometric u_offset, geometric v_offset) const;
};

}  // namespace iris

#endif  // _IRIS_TEXTURE_COORDINATES_
