#ifndef _IRIS_BOUNDING_BOX_
#define _IRIS_BOUNDING_BOX_

#include <algorithm>
#include <limits>
#include <optional>
#include <span>

#include "iris/point.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {

struct BoundingBox final {
  class Builder {
   public:
    Builder();

    void Add(const BoundingBox& bounds) noexcept;
    void Add(const Point& point) noexcept;
    void Reset() noexcept;

    BoundingBox Build() const noexcept;

   private:
    geometric min_x_, min_y_, min_z_;
    geometric max_x_, max_y_, max_z_;
    bool contains_points_ = false;
  };

  template <typename T, typename... Rest>
  explicit BoundingBox(const T& first, const Rest&... rest) noexcept
      : lower(Min(first, rest...)), upper(Max(first, rest...)) {}

  explicit BoundingBox(std::span<const Point> points) noexcept
      : lower(Min(points)), upper(Max(points)) {}

  BoundingBox(const BoundingBox&) noexcept = default;

  bool Empty() const {
    return (lower.x == upper.x) + (lower.y == upper.y) + (lower.z == upper.z) >=
           2;
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

  std::optional<Intersection> Intersect(const Ray& ray) const {
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

  static Point Min(std::span<const Point> points);

  static Point Max(const Point& point) { return point; }

  static Point Max(const Point& p0, const Point& p1) {
    return Point(std::max(p0.x, p1.x), std::max(p0.y, p1.y),
                 std::max(p0.z, p1.z));
  }

  template <typename T, typename... Rest>
  static Point Max(const T& first, const Rest&... rest) {
    return Max(first, Max(rest...));
  }

  static Point Max(std::span<const Point> points);

  friend BoundingBox Intersect(const BoundingBox& b0, const BoundingBox& b1);
};

BoundingBox Intersect(const BoundingBox& b0, const BoundingBox& b1);
BoundingBox Join(const BoundingBox& b0, const BoundingBox& b1);

}  // namespace iris

#endif  // _IRIS_BOUNDING_BOX_