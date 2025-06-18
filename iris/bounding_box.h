#ifndef _IRIS_BOUNDING_BOX_
#define _IRIS_BOUNDING_BOX_

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

  Point Center() const {
    geometric x = static_cast<visual_t>(0.5) * (upper.x + lower.x);
    geometric y = static_cast<visual_t>(0.5) * (upper.y + lower.y);
    geometric z = static_cast<visual_t>(0.5) * (upper.z + lower.z);
    return Point(x, y, z);
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

  bool Intersects(const Ray& ray, geometric_t minimum_distance,
                  geometric_t maximum_distance) const {
    geometric_t inverse_direction_x =
        static_cast<geometric_t>(1.0) / ray.direction.x;
    geometric_t inverse_direction_y =
        static_cast<geometric_t>(1.0) / ray.direction.y;
    geometric_t inverse_direction_z =
        static_cast<geometric_t>(1.0) / ray.direction.z;

    geometric_t tx0 = (lower.x - ray.origin.x) * inverse_direction_x;
    geometric_t tx1 = (upper.x - ray.origin.x) * inverse_direction_x;
    geometric_t min = Min(tx0, tx1);
    geometric_t max = Max(tx0, tx1);

    geometric_t ty0 = (lower.y - ray.origin.y) * inverse_direction_y;
    geometric_t ty1 = (upper.y - ray.origin.y) * inverse_direction_y;
    min = Max(min, Min(ty0, ty1));
    max = Min(max, Max(ty0, ty1));

    geometric_t tz0 = (lower.z - ray.origin.z) * inverse_direction_z;
    geometric_t tz1 = (upper.z - ray.origin.z) * inverse_direction_z;
    min = Max(min, Min(tz0, tz1));
    max = Min(max, Max(tz0, tz1));

    return min <= max && minimum_distance < max && maximum_distance > min;
  }

  bool operator==(const BoundingBox&) const = default;

  const Point lower;
  const Point upper;

 private:
  static geometric_t Min(geometric_t v0, geometric_t v1) {
    return (v0 < v1) ? v0 : v1;
  }

  static geometric_t Max(geometric_t v0, geometric_t v1) {
    return (v0 > v1) ? v0 : v1;
  }

  static Point Min(const Point& point) { return point; }

  static Point Min(const Point& p0, const Point& p1) {
    return Point(Min(p0.x, p1.x), Min(p0.y, p1.y), Min(p0.z, p1.z));
  }

  template <typename T, typename... Rest>
  static Point Min(const T& first, const Rest&... rest) {
    return Min(first, Min(rest...));
  }

  static Point Min(std::span<const Point> points);

  static Point Max(const Point& point) { return point; }

  static Point Max(const Point& p0, const Point& p1) {
    return Point(Max(p0.x, p1.x), Max(p0.y, p1.y), Max(p0.z, p1.z));
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
