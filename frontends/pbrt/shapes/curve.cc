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
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

using ::iris::geometry::MakeFlatCurve;
using ::pbrt_proto::v3::Shape;

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

  if (curve.basis() != Shape::Curve::BEZIER) {
    std::cerr << "ERROR: Unsupported value for parameter: basis" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (curve.degree() != 3) {
    std::cerr << "ERROR: Unsupported value for parameter: degree" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (curve.type() != Shape::Curve::FLAT) {
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

  if (curve.splitdepth() < 0 || curve.splitdepth() >= 32) {
    std::cerr << "ERROR: Out of range value for parameter: splitdepth"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  uint32_t num_segments = 1u << curve.splitdepth();

  std::vector<Point> points;
  for (const auto& p : curve.p()) {
    Point model_point(static_cast<geometric>(p.x()),
                      static_cast<geometric>(p.y()),
                      static_cast<geometric>(p.z()));
    points.push_back(model_to_world.Multiply(model_point));
  }

  std::vector<ReferenceCounted<Geometry>> result;
  for (size_t i = 0; points.size() - i >= 4; i += 4) {
    std::vector<ReferenceCounted<Geometry>> segments = MakeFlatCurve(
        {points[i], points[i + 1], points[i + 2], points[i + 3]}, num_segments,
        start_width, end_width, front_material, front_normal_map);
    result.insert(result.end(), segments.begin(), segments.end());
  }

  return std::make_pair(result, Matrix::Identity());
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
