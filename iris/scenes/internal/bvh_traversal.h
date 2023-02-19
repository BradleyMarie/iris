#ifndef _IRIS_SCENES_INTERNAL_BVH_TRAVERSAL_
#define _IRIS_SCENES_INTERNAL_BVH_TRAVERSAL_

#include "iris/intersector.h"
#include "iris/scene_objects.h"
#include "iris/scenes/internal/bvh_node.h"

namespace iris {
namespace scenes {
namespace internal {

void Intersect(const BVHNode& bvh, const SceneObjects& scene_objects,
               const Ray& ray, Intersector& intersector);

}  // namespace internal
}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_INTERNAL_BVH_NODE_