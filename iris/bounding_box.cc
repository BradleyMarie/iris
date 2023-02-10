#include "iris/bounding_box.h"

namespace iris {

Point BoundingBox::Min(std::span<const Point> points) {
  if (points.empty()) {
    return Point(0.0, 0.0, 0.0);
  }

  geometric min_x = std::numeric_limits<geometric>::infinity();
  geometric min_y = std::numeric_limits<geometric>::infinity();
  geometric min_z = std::numeric_limits<geometric>::infinity();
  for (const auto& point : points) {
    min_x = std::min(min_x, point.x);
    min_y = std::min(min_y, point.y);
    min_z = std::min(min_z, point.z);
  }

  return Point(min_x, min_y, min_z);
}

Point BoundingBox::Max(std::span<const Point> points) {
  if (points.empty()) {
    return Point(0.0, 0.0, 0.0);
  }

  geometric max_x = -std::numeric_limits<geometric>::infinity();
  geometric max_y = -std::numeric_limits<geometric>::infinity();
  geometric max_z = -std::numeric_limits<geometric>::infinity();
  for (const auto& point : points) {
    max_x = std::max(max_x, point.x);
    max_y = std::max(max_y, point.y);
    max_z = std::max(max_z, point.z);
  }

  return Point(max_x, max_y, max_z);
}

BoundingBox Intersect(const BoundingBox& b0, const BoundingBox& b1) {
  Point highest_low = BoundingBox::Max(b0.lower, b1.lower);
  Point lowest_high = BoundingBox::Min(b0.upper, b1.upper);

  if (lowest_high.x <= highest_low.x || lowest_high.y <= highest_low.y ||
      lowest_high.z <= highest_low.z) {
    return BoundingBox(Point(0.0, 0.0, 0.0));
  }

  return BoundingBox(highest_low, lowest_high);
}

BoundingBox Join(const BoundingBox& b0, const BoundingBox& b1) {
  if (b0.Empty()) {
    return b1;
  }

  if (b1.Empty()) {
    return b0;
  }

  return BoundingBox(b0.lower, b1.lower, b0.upper, b1.upper);
}

}  // namespace iris