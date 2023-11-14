#ifndef _FRONTENDS_PBRT_SHAPE_TRIANGLEMESH_
#define _FRONTENDS_PBRT_SHAPE_TRIANGLEMESH_

#include <vector>

#include "frontends/pbrt/object_builder.h"
#include "iris/geometry.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"

namespace iris::pbrt_frontend::shapes {

extern const std::unique_ptr<const ObjectBuilder<
    std::pair<std::vector<ReferenceCounted<Geometry>>, Matrix>,
    const ReferenceCounted<iris::Material>&,
    const ReferenceCounted<iris::NormalMap>&,
    const ReferenceCounted<iris::NormalMap>&,
    const ReferenceCounted<EmissiveMaterial>&,
    const ReferenceCounted<EmissiveMaterial>&, const Matrix&>>
    g_trianglemesh_builder;

}  // namespace iris::pbrt_frontend::shapes

#endif  // _FRONTENDS_PBRT_SHAPE_TRIANGLEMESH_