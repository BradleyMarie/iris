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
  CubicBezierCurve(const Point points[4], geometric half_start_width,
                   geometric half_end_width);

  BoundingBox ComputeBounds(const Matrix* matrix) const;
  BoundingBox ComputeBounds(const Matrix& matrix) const;
  BoundingBox ComputeBounds() const;

  bool MaybeIntersects(geometric_t minimum_distance,
                       geometric_t maximum_distance) const;

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

  geometric MaxHalfWidth() const {
    return half_widths_[0] > half_widths_[3] ? half_widths_[0]
                                             : half_widths_[3];
  }

  Point operator[](size_t index) const { return points_[index]; }

 private:
  alignas(16) Point points_[4];
  geometric half_widths_[4];
};

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_INTERNAL_CUBIC_BEZIER_CURVE_
