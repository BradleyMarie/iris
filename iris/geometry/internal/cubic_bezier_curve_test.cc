#include "iris/geometry/internal/cubic_bezier_curve.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/matrix.h"
#include "iris/point.h"

namespace iris {
namespace geometry {
namespace {

TEST(CubicBezierCurve, Create) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                     Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)};

  CubicBezierCurve curve(points, 0.0, 1.0);
  EXPECT_EQ(points[0], curve[0]);
  EXPECT_EQ(points[1], curve[1]);
  EXPECT_EQ(points[2], curve[2]);
  EXPECT_EQ(points[3], curve[3]);
}

TEST(CubicBezierCurve, MaxWidth) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                     Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)};

  EXPECT_EQ(1.0, CubicBezierCurve(points, 0.0, 1.0).MaxWidth());
  EXPECT_EQ(2.0, CubicBezierCurve(points, 2.0, 1.0).MaxWidth());
}

TEST(CubicBezierCurve, ComputeBounds) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                     Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)};

  EXPECT_EQ(BoundingBox(Point(-1.0, -1.0, -1.0), Point(4.0, 2.0, 1.0)),
            CubicBezierCurve(points, 0.0, 1.0).ComputeBounds());

  Matrix matrix = Matrix::Translation(1.0, 0.0, 0.0).value();
  EXPECT_EQ(BoundingBox(Point(0.0, -1.0, -1.0), Point(5.0, 2.0, 1.0)),
            CubicBezierCurve(points, 0.0, 1.0).ComputeBounds(&matrix));
}

TEST(CubicBezierCurve, ComputeFlatness) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                     Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)};

  EXPECT_EQ(2.0, CubicBezierCurve(points, 0.0, 1.0).ComputeFlatness());
}

TEST(CubicBezierCurve, Diagonal) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(2.0, 1.0, 0.0),
                     Point(4.0, 0.0, 0.0), Point(6.0, 1.0, 0.0)};
  EXPECT_EQ(Vector(6.0, 1.0, 0.0),
            CubicBezierCurve(points, 0.0, 1.0).Diagonal());
}

TEST(CubicBezierCurve, Evaluate) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(2.0, 1.0, 0.0),
                     Point(4.0, 0.0, 0.0), Point(6.0, 1.0, 0.0)};
  CubicBezierCurve curve(points, 0.0, 1.0);

  geometric_t derivative_x, derivative_y, derivative_z;
  auto [point0, width0] =
      curve.Evaluate(0.0, &derivative_x, &derivative_y, &derivative_z);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), point0);
  EXPECT_EQ(0.0, width0);
  EXPECT_EQ(6.0, derivative_x);
  EXPECT_EQ(3.0, derivative_y);
  EXPECT_EQ(0.0, derivative_z);

  auto [point1, width1] = curve.Evaluate(0.25);
  EXPECT_EQ(Point(1.5, 0.4375, 0.0), point1);
  EXPECT_EQ(0.25, width1);

  auto [point2, width2] = curve.Evaluate(0.5);
  EXPECT_EQ(Point(3.0, 0.5, 0.0), point2);
  EXPECT_EQ(0.5, width2);

  auto [point3, width3] = curve.Evaluate(0.75);
  EXPECT_EQ(Point(4.5, 0.5625, 0.0), point3);
  EXPECT_EQ(0.75, width3);

  auto [point4, width4] = curve.Evaluate(1.0);
  EXPECT_EQ(Point(6.0, 1.0, 0.0), point4);
  EXPECT_EQ(1.0, width4);
}

TEST(CubicBezierCurve, EvaluateDerivative) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(2.0, 1.0, 0.0),
                     Point(4.0, 0.0, 0.0), Point(6.0, 1.0, 0.0)};
  EXPECT_EQ(Vector(6.0, 3.0, 0.0),
            CubicBezierCurve(points, 0.0, 1.0).EvaluateDerivative(0.0));
}

TEST(CubicBezierCurve, ExtractSegment) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(2.0, 1.0, 0.0),
                     Point(4.0, 0.0, 0.0), Point(6.0, 1.0, 0.0)};
  CubicBezierCurve curve =
      CubicBezierCurve(points, 0.0, 1.0).ExtractSegment(0.0, 0.25);

  auto [point0, width0] = curve.Evaluate(0.0);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), point0);
  EXPECT_EQ(0.0, width0);

  auto [point1, width1] = curve.Evaluate(1.0);
  EXPECT_EQ(Point(1.5, 0.4375, 0.0), point1);
  EXPECT_EQ(0.25, width1);
}

TEST(CubicBezierCurve, Subdivide) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(2.0, 1.0, 0.0),
                     Point(4.0, 0.0, 0.0), Point(6.0, 1.0, 0.0)};
  auto [curve0, curve1] = CubicBezierCurve(points, 0.0, 1.0).Subdivide();

  auto [point0, width0] = curve0.Evaluate(0.0);
  EXPECT_EQ(Point(0.0, 0.0, 0.0), point0);
  EXPECT_EQ(0.0, width0);

  auto [point1, width1] = curve0.Evaluate(1.0);
  EXPECT_EQ(Point(3.0, 0.5, 0.0), point1);
  EXPECT_EQ(0.5, width1);

  auto [point2, width2] = curve1.Evaluate(0.0);
  EXPECT_EQ(Point(3.0, 0.5, 0.0), point2);
  EXPECT_EQ(0.5, width2);

  auto [point3, width3] = curve1.Evaluate(1.0);
  EXPECT_EQ(Point(6.0, 1.0, 0.0), point3);
  EXPECT_EQ(1.0, width3);
}

TEST(CubicBezierCurve, InverseTransform) {
  Point points[4] = {Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 0.0),
                     Point(2.0, 0.0, 0.0), Point(3.0, 1.0, 0.0)};

  Matrix transform = Matrix::Translation(1.0, 0.0, 0.0).value();
  EXPECT_EQ(BoundingBox(Point(-2.0, -1.0, -1.0), Point(3.0, 2.0, 1.0)),
            CubicBezierCurve(points, 0.0, 1.0)
                .InverseTransform(transform)
                .ComputeBounds());
}

}  // namespace
}  // namespace geometry
}  // namespace iris
