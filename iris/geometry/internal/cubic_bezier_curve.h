#ifndef _IRIS_GEOMETRY_INTERNAL_CUBIC_BEZIER_CURVE_
#define _IRIS_GEOMETRY_INTERNAL_CUBIC_BEZIER_CURVE_

#include <utility>

#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/point.h"

namespace iris {
namespace geometry {

class CubicBezierCurve {
 public:
  CubicBezierCurve(const Point points[4], geometric start_width,
                   geometric end_width)
      : points_{points[0], points[1], points[2], points[3]},
        start_width_(start_width),
        end_width_(end_width) {}

  BoundingBox ComputeBounds(const Matrix* matrix = nullptr) const;
  geometric_t ComputeFlatness() const;
  Vector Diagonal() const;

  std::pair<Point, geometric_t> Evaluate(
      geometric_t u, geometric_t* derivative_x = nullptr,
      geometric_t* derivative_y = nullptr,
      geometric_t* derivative_z = nullptr) const;
  Vector EvaluateDerivative(geometric_t u) const;

  CubicBezierCurve ExtractSegment(geometric_t start, geometric_t end) const;
  std::pair<CubicBezierCurve, CubicBezierCurve> Subdivide() const;

  CubicBezierCurve InverseTransform(const Matrix& transform) const;

  geometric MaxWidth() const {
    return start_width_ > end_width_ ? start_width_ : end_width_;
  }

  const Point& operator[](size_t index) const { return points_[index]; }

 private:
  Point points_[4];
  geometric start_width_;
  geometric end_width_;
};

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_INTERNAL_CUBIC_BEZIER_CURVE_
