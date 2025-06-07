#ifndef _IRIS_GEOMETRY_BVH_AGGREGATE_
#define _IRIS_GEOMETRY_BVH_AGGREGATE_

#include <vector>

#include "iris/geometry.h"
#include "iris/reference_counted.h"

namespace iris {
namespace geometry {

ReferenceCounted<Geometry> AllocateBVHAggregate(
    std::vector<ReferenceCounted<Geometry>> geometry);

}  // namespace geometry
}  // namespace iris

#endif  // _IRIS_GEOMETRY_BVH_AGGREGATE_
