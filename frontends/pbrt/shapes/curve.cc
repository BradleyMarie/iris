#include "frontends/pbrt/shapes/curve.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>

#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/geometry/curve.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/pbrt.pb.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::iris::geometry::MakeCylindricalCubicBezierCurve;
using ::iris::geometry::MakeFlatCubicBezierCurve;
using ::pbrt_proto::v3::Shape;

Point Lerp(const Point& p0, const Point& p1, geometric_t t) {
  return Point(std::lerp(p0.x, p1.x, t), std::lerp(p0.y, p1.y, t),
               std::lerp(p0.z, p1.z, t));
}

std::array<Point, 4> ComputeCubicSegment(Shape::Curve::Basis basis,
                                         std::span<const Point> points,
                                         size_t segment) {
  if (basis == Shape::Curve::BEZIER) {
    return {points[segment * 3u + 0], points[segment * 3u + 1],
            points[segment * 3u + 2], points[segment * 3u + 3]};
  }

  Point p012 = points[segment + 0];
  Point p123 = points[segment + 1];
  Point p234 = points[segment + 2];
  Point p345 = points[segment + 3];

  Point p122 = Lerp(p012, p123, 2.0 / 3.0);
  Point p223 = Lerp(p123, p234, 1.0 / 3.0);
  Point p233 = Lerp(p123, p234, 2.0 / 3.0);
  Point p334 = Lerp(p234, p345, 1.0 / 3.0);

  Point p222 = Lerp(p122, p223, 0.5);
  Point p333 = Lerp(p233, p334, 0.5);

  return {p222, p223, p233, p333};
}

}  // namespace

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> MakeCurve(
    const Shape::Curve& curve, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map) {
  if (curve.p_size() < 4) {
    std::cerr << "ERROR: Incorrect number of values for parameter: p"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (curve.type() == Shape::Curve::RIBBON) {
    std::cerr << "ERROR: Unsupported value for parameter: type" << std::endl;
    exit(EXIT_FAILURE);
  }

  geometric start_width = curve.width();
  if (curve.has_width0()) {
    start_width = curve.width0();
  }

  geometric end_width = curve.width();
  if (curve.has_width1()) {
    end_width = curve.width1();
  }

  if (curve.splitdepth() >= 32) {
    std::cerr << "ERROR: Out of range value for parameter: splitdepth"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  uint32_t splits_per_segment = 1u << curve.splitdepth();

  std::vector<Point> points;
  for (const auto& p : curve.p()) {
    Point model_point(static_cast<geometric>(p.x()),
                      static_cast<geometric>(p.y()),
                      static_cast<geometric>(p.z()));
    points.push_back(model_to_world.Multiply(model_point));
  }

  int degree = 0;
  switch (curve.degree()) {
    case Shape::Curve::THREE:
      degree = 3;
      break;
    case Shape::Curve::FOUR:
      std::cerr << "ERROR: Unsupported value for parameter: degree"
                << std::endl;
      exit(EXIT_FAILURE);
      break;
  }

  size_t num_segments = 0;
  switch (curve.basis()) {
    case Shape::Curve::BEZIER:
      if (curve.p_size() >= curve.degree() + 1 &&
          (curve.p_size() - degree - 1) % degree == 0) {
        num_segments = static_cast<size_t>((curve.p_size() - 1) / degree);
      } else {
        std::cerr << "WARNING: Invalid number of values to parameter: p"
                  << std::endl;
      }
      break;
    case Shape::Curve::BSPLINE:
      if (curve.p_size() > 3) {
        num_segments = static_cast<size_t>(curve.p_size() - degree);
      } else {
        std::cerr << "WARNING: Invalid number of values to parameter: p"
                  << std::endl;
      }
      break;
  }

  std::vector<ReferenceCounted<Geometry>> result;
  for (size_t i = 0; i < num_segments; i++) {
    std::array<Point, 4> control_points =
        ComputeCubicSegment(curve.basis(), points, i);

    geometric start_u =
        static_cast<double>(i) / static_cast<double>(num_segments);
    geometric end_u =
        static_cast<double>(i + 1) / static_cast<double>(num_segments);

    std::vector<ReferenceCounted<Geometry>> segments;
    switch (curve.type()) {
      case Shape::Curve::FLAT:
        segments = MakeFlatCubicBezierCurve(
            control_points, splits_per_segment,
            std::lerp(start_width, end_width, start_u),
            std::lerp(start_width, end_width, end_u), start_u, end_u,
            front_material, front_normal_map);
        break;
      case Shape::Curve::CYLINDER:
        segments = MakeCylindricalCubicBezierCurve(
            control_points, splits_per_segment, start_width, end_width, start_u,
            end_u, front_material, front_normal_map);
        break;
      case Shape::Curve::RIBBON:
        break;
    }

    result.insert(result.end(), segments.begin(), segments.end());
  }

  return std::make_pair(result, Matrix::Identity());
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
