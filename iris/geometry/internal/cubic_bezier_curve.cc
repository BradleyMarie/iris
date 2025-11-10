#include "iris/geometry/internal/cubic_bezier_curve.h"

#include <algorithm>
#include <cmath>
#include <expected>
#include <utility>

#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/matrix.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {
namespace {

Point Lerp(const Point& p0, const Point& p1, geometric_t t) {
  return Point(std::lerp(p0.x, p1.x, t), std::lerp(p0.y, p1.y, t),
               std::lerp(p0.z, p1.z, t));
}

Point Sum(const Point& p0, const Point& p1) {
  return Point(p0.x + p1.x, p0.y + p1.y, p0.z + p1.z);
}

template <typename T, typename... Rest>
Point Sum(const T& first, const T& second, const Rest&... rest) {
  return Sum(Sum(first, second), rest...);
}

Point BlossomBezier(const Point points[4], geometric u0, geometric u1,
                    geometric u2) {
  Point a[3] = {Lerp(points[0], points[1], u0), Lerp(points[1], points[2], u0),
                Lerp(points[2], points[3], u0)};
  Point b[2] = {Lerp(a[0], a[1], u1), Lerp(a[1], a[2], u1)};
  return Lerp(b[0], b[1], u2);
}

}  // namespace

BoundingBox CubicBezierCurve::ComputeBounds(const Matrix* transform) const {
  geometric_t max_width = std::max(start_width_, end_width_);

  BoundingBox::Builder builder;
  if (transform) {
    Vector offsets[8] = {
        Vector(max_width, max_width, max_width),
        Vector(max_width, max_width, -max_width),
        Vector(max_width, -max_width, max_width),
        Vector(max_width, -max_width, -max_width),
        Vector(-max_width, max_width, max_width),
        Vector(-max_width, max_width, -max_width),
        Vector(-max_width, -max_width, max_width),
        Vector(-max_width, -max_width, -max_width),
    };

    for (const Point& point : points_) {
      for (const Vector& offset : offsets) {
        Point offset_point = point + offset;
        builder.Add(transform->Multiply(offset_point));
      }
    }
  } else {
    Vector offset(max_width, max_width, max_width);
    for (const Point& point : points_) {
      builder.Add(point + offset);
      builder.Add(point - offset);
    }
  }

  return builder.Build();
}

geometric_t CubicBezierCurve::ComputeFlatness() const {
  geometric_t flatness = static_cast<geometric_t>(0.0);

  for (size_t i = 0; i < 2; ++i) {
    geometric_t difference_x = std::abs(
        points_[i].x - static_cast<geometric_t>(2.0) * points_[i + 1].x +
        points_[i + 2].x);
    flatness = std::max(flatness, difference_x);

    geometric_t difference_y = std::abs(
        points_[i].y - static_cast<geometric_t>(2.0) * points_[i + 1].y +
        points_[i + 2].y);
    flatness = std::max(flatness, difference_y);

    geometric_t difference_z = std::abs(
        points_[i].z - static_cast<geometric_t>(2.0) * points_[i + 1].z +
        points_[i + 2].z);
    flatness = std::max(flatness, difference_z);
  }

  return flatness;
}

Vector CubicBezierCurve::Diagonal() const { return points_[3] - points_[0]; }

std::pair<Point, geometric_t> CubicBezierCurve::Evaluate(
    geometric_t u, geometric_t* derivative_x, geometric_t* derivative_y,
    geometric_t* derivative_z) const {
  Point a[3] = {Lerp(points_[0], points_[1], u),
                Lerp(points_[1], points_[2], u),
                Lerp(points_[2], points_[3], u)};
  Point b[2] = {Lerp(a[0], a[1], u), Lerp(a[1], a[2], u)};

  if (derivative_x) {
    *derivative_x = static_cast<geometric_t>(3.0) * (b[1].x - b[0].x);
  }

  if (derivative_y) {
    *derivative_y = static_cast<geometric_t>(3.0) * (b[1].y - b[0].y);
  }

  if (derivative_z) {
    *derivative_z = static_cast<geometric_t>(3.0) * (b[1].z - b[0].z);
  }

  return std::make_pair(Lerp(b[0], b[1], u),
                        std::lerp(start_width_, end_width_, u));
}

Vector CubicBezierCurve::EvaluateDerivative(geometric_t u) const {
  Point a[3] = {Lerp(points_[0], points_[1], u),
                Lerp(points_[1], points_[2], u),
                Lerp(points_[2], points_[3], u)};
  Point b[2] = {Lerp(a[0], a[1], u), Lerp(a[1], a[2], u)};
  return static_cast<geometric_t>(3.0) * (b[1] - b[0]);
}

CubicBezierCurve CubicBezierCurve::ExtractSegment(geometric_t start,
                                                  geometric_t end) const {
  Point points[4] = {
      BlossomBezier(points_, start, start, start),
      BlossomBezier(points_, start, start, end),
      BlossomBezier(points_, start, end, end),
      BlossomBezier(points_, end, end, end),
  };

  return CubicBezierCurve(points, std::lerp(start_width_, end_width_, start),
                          std::lerp(start_width_, end_width_, end));
}

std::pair<CubicBezierCurve, CubicBezierCurve> CubicBezierCurve::Subdivide()
    const {
  Point points[7] = {points_[0],
                     Sum(points_[0] * static_cast<geometric_t>(0.5),
                         points_[1] * static_cast<geometric_t>(0.5)),
                     Sum(points_[0] * static_cast<geometric_t>(0.25),
                         points_[1] * static_cast<geometric_t>(0.5),
                         points_[2] * static_cast<geometric_t>(0.25)),
                     Sum(points_[0] * static_cast<geometric_t>(0.125),
                         points_[1] * static_cast<geometric_t>(0.375),
                         points_[2] * static_cast<geometric_t>(0.375),
                         points_[3] * static_cast<geometric_t>(0.125)),
                     Sum(points_[1] * static_cast<geometric_t>(0.25),
                         points_[2] * static_cast<geometric_t>(0.5),
                         points_[3] * static_cast<geometric_t>(0.25)),
                     Sum(points_[2] * static_cast<geometric_t>(0.5),
                         points_[3] * static_cast<geometric_t>(0.5)),
                     points_[3]};

  geometric_t middle_width =
      static_cast<geometric_t>(0.5) * (start_width_ + end_width_);

  return std::make_pair(CubicBezierCurve(points, start_width_, middle_width),
                        CubicBezierCurve(points + 3, middle_width, end_width_));
}

CubicBezierCurve CubicBezierCurve::InverseTransform(
    const Matrix& transform) const {
  Point points[4] = {
      transform.InverseMultiply(points_[0]),
      transform.InverseMultiply(points_[1]),
      transform.InverseMultiply(points_[2]),
      transform.InverseMultiply(points_[3]),
  };

  return CubicBezierCurve(points, start_width_, end_width_);
}

}  // namespace geometry
}  // namespace iris
