#ifndef _IRIS_TEXTURE_COORDINATES_
#define _IRIS_TEXTURE_COORDINATES_

#include <array>
#include <optional>

#include "iris/float.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {

struct TextureCoordinates final {
  const Point hit_point;

  struct HitPointDifferentials {
    Vector dhit_point_dx;
    Vector dhit_point_dy;
  };

  const std::optional<HitPointDifferentials> hit_point_differentials;

  const std::array<geometric_t, 2> uv;

  struct UVDifferentials {
    geometric_t du_dx;
    geometric_t du_dy;
    geometric_t dv_dx;
    geometric_t dv_dy;
  };

  const std::optional<UVDifferentials> uv_differentials;

  TextureCoordinates Scale(geometric u_scale, geometric v_scale,
                           geometric u_offset, geometric v_offset) const;
};

}  // namespace iris

#endif  // _IRIS_TEXTURE_COORDINATES_
