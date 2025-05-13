#ifndef _FRONTENDS_PBRT_SHAPE_SPHERE_
#define _FRONTENDS_PBRT_SHAPE_SPHERE_

#include <utility>
#include <vector>

#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/reference_counted.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {

std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix> MakeSphere(
    const pbrt_proto::v3::Shape::Sphere& sphere, const Matrix& model_to_world,
    const ReferenceCounted<Material>& front_material,
    const ReferenceCounted<Material>& back_material,
    const ReferenceCounted<EmissiveMaterial>& front_emissive_material,
    const ReferenceCounted<EmissiveMaterial>& back_emissive_material,
    const ReferenceCounted<NormalMap>& front_normal_map,
    const ReferenceCounted<NormalMap>& back_normal_map);

}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_SHAPE_SPHERE_
