#ifndef _IRIS_COLOR_
#define _IRIS_COLOR_

#include "iris/float.h"

namespace iris {

struct Color final {
  enum Space {
    CIE_XYZ = 0,
    LINEAR_SRGB = 1,
  };

#ifdef NDEBUG
  Color(visual_t c0, visual_t c1, visual_t c2, Space space) noexcept
      : x(c0), y(c1), z(c2), space(space) {}
#else
  Color(visual_t c0, visual_t c1, visual_t c2, Space space) noexcept;
#endif  // NDEBUG

  Color ConvertTo(Space target) const;

  visual_t Luma() const;

  union {
    struct {
      const visual_t x;
      const visual_t y;
      const visual_t z;
    };
    struct {
      const visual_t r;
      const visual_t g;
      const visual_t b;
    };
    struct {
      const visual_t c0;
      const visual_t c1;
      const visual_t c2;
    };
  };

  const Space space;
};

static inline bool operator==(const Color& lhs, const Color& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z &&
         lhs.space == rhs.space;
}

}  // namespace iris

#endif  // _IRIS_COLOR_
