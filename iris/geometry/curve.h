#ifndef _IRIS_GEOMETRY_CURVE_
#define _IRIS_GEOMETRY_CURVE_

#include <array>
#include <cstdint>
#include <vector>

#include "iris/emissive_material.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/point.h"
#include "iris/reference_counted.h"

namespace iris {
namespace geometry {

std::vector<ReferenceCounted<Geometry>> MakeFlatCurve(
    const std::array<Point, 4>& control_points, uint32_t num_segments,
    geometric start_width, geometric end_width,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<NormalMap> front_normal_map);

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_CURVE_
