#ifndef _IRIS_BOUNDING_BOX_
#define _IRIS_BOUNDING_BOX_

#include <algorithm>
#include <limits>
#include <optional>

#include "iris/point.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {

// Forward Declarations
class BoundingBox;
static inline std::optional<BoundingBox> Intersection(const BoundingBox& b0,
                                                      const BoundingBox& b1);

struct BoundingBox final {
  template <typename T, typename... Rest>
  explicit BoundingBox(const T& first, const Rest&... rest) noexcept
      : lower(Min(first, rest...)), upper(Max(first, rest...)) {}

  BoundingBox(const BoundingBox&) noexcept = default;

  BoundingBox Envelop(const Point& point) const {
    return BoundingBox(Min(upper, point), Max(upper, point));
  }

  geometric_t SurfaceArea() const {
    Vector diagonal = upper - lower;
    geometric_t area0 = diagonal.x * diagonal.z;
    geometric_t area1 = diagonal.y * diagonal.x;
    geometric_t area2 = diagonal.z * diagonal.y;
    return static_cast<geometric_t>(2.0) * (area0 + area1 + area2);
  }

  struct Intersection {
    const geometric_t inverse_direction[3];
    const geometric_t begin;
    const geometric_t end;
  };

  std::optional<Intersection> Intersects(const Ray& ray) {
    geometric_t inverse_direction_x =
        static_cast<geometric_t>(1.0) / ray.direction.x;
    geometric_t inverse_direction_y =
        static_cast<geometric_t>(1.0) / ray.direction.y;
    geometric_t inverse_direction_z =
        static_cast<geometric_t>(1.0) / ray.direction.z;

    geometric_t min = 0.0;
    geometric_t max = std::numeric_limits<geometric_t>::infinity();

    geometric_t tx0 = (lower.x - ray.origin.x) * inverse_direction_x;
    geometric_t tx1 = (upper.x - ray.origin.x) * inverse_direction_x;
    min = std::max(min, std::min(tx0, tx1));
    max = std::min(max, std::max(tx0, tx1));

    geometric_t ty0 = (lower.y - ray.origin.y) * inverse_direction_y;
    geometric_t ty1 = (upper.y - ray.origin.y) * inverse_direction_y;
    min = std::max(min, std::min(ty0, ty1));
    max = std::min(max, std::max(ty0, ty1));

    geometric_t tz0 = (lower.z - ray.origin.z) * inverse_direction_z;
    geometric_t tz1 = (upper.z - ray.origin.z) * inverse_direction_z;
    min = std::max(min, std::min(tz0, tz1));
    max = std::min(max, std::max(tz0, tz1));

    if (max < min) {
      return std::nullopt;
    }

    return Intersection{
        {inverse_direction_x, inverse_direction_y, inverse_direction_z},
        min,
        max};
  }

  const Point lower;
  const Point upper;

 private:
  static Point Min(const Point& point) { return point; }

  static Point Min(const Point& p0, const Point& p1) {
    return Point(std::min(p0.x, p1.x), std::min(p0.y, p1.y),
                 std::min(p0.z, p1.z));
  }

  template <typename T, typename... Rest>
  static Point Min(const T& first, const Rest&... rest) {
    return Min(first, Min(rest...));
  }

  static Point Max(const Point& point) { return point; }

  static Point Max(const Point& p0, const Point& p1) {
    return Point(std::max(p0.x, p1.x), std::max(p0.y, p1.y),
                 std::max(p0.z, p1.z));
  }

  template <typename T, typename... Rest>
  static Point Max(const T& first, const Rest&... rest) {
    return Max(first, Max(rest...));
  }

  friend std::optional<BoundingBox> Intersection(const BoundingBox& b0,
                                                 const BoundingBox& b1);
};

static inline std::optional<BoundingBox> Intersection(const BoundingBox& b0,
                                                      const BoundingBox& b1) {
  Point highest_low = BoundingBox::Max(b0.lower, b1.lower);
  Point lowest_high = BoundingBox::Min(b0.upper, b1.upper);

  if (lowest_high.x <= highest_low.x || lowest_high.y <= highest_low.y ||
      lowest_high.z <= highest_low.z) {
    return std::nullopt;
  }

  return BoundingBox(highest_low, lowest_high);
}

static inline BoundingBox Union(const BoundingBox& b0, const BoundingBox& b1) {
  return BoundingBox(b0.lower, b1.lower, b0.upper, b1.upper);
}

}  // namespace iris

#endif  // _IRIS_BOUNDING_BOX_