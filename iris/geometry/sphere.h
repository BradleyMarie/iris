#ifndef _IRIS_GEOMETRY_SPHERE_
#define _IRIS_GEOMETRY_SPHERE_

#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/integer.h"
#include "iris/material.h"
#include "iris/normal_map.h"
#include "iris/point.h"
#include "iris/reference_counted.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {

ReferenceCounted<Geometry> AllocateSphere(
    const Point& center, geometric radius,
    ReferenceCounted<Material> back_material,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<EmissiveMaterial> front_emissive_material,
    ReferenceCounted<EmissiveMaterial> back_emissive_material,
    ReferenceCounted<NormalMap> front_normal_map,
    ReferenceCounted<NormalMap> back_normal_map);

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_SPHERE_