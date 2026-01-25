#include "frontends/pbrt/shapes/sphere.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>

#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/geometry/sphere.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

using ::iris::geometry::AllocateSphere;
using ::pbrt_proto::v3::Shape;

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> MakeSphere(
    const Shape::Sphere& sphere, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map) {
  if (!std::isfinite(sphere.radius()) || sphere.radius() < 0.0) {
    std::cerr << "ERROR: Out of range value for parameter: radius" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (sphere.radius() == 0.0) {
    return std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>(
        {}, model_to_world);
  }

  ReferenceCounted<Geometry> geometry = AllocateSphere(
      Point(0.0, 0.0, 0.0), static_cast<geometric>(sphere.radius()),
      front_material, back_material, front_emissive_material,
      back_emissive_material, front_normal_map, back_normal_map);

  return std::make_pair(std::vector<ReferenceCounted<Geometry>>({geometry}),
                        model_to_world);
}

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris
