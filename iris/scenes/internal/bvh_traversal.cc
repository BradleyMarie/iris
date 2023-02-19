#include "iris/scenes/internal/bvh_traversal.h"

#include "iris/scenes/internal/bvh_builder.h"

namespace iris {
namespace scenes {
namespace internal {

void Intersect(const BVHNode& bvh, const SceneObjects& scene_objects,
               const Ray& ray, Intersector& intersector) {
  const BVHNode* work_list[internal::kMaxBvhDepth];
  const BVHNode* current = &bvh;
  size_t queue_size = 0;
  for (;;) {
    auto intersection = current->Intersects(ray);
    if (intersection && intersection->end > intersector.MinimumDistance() &&
        intersection->begin < intersector.MaximumDistance()) {
      if (current->HasChildren()) {
        auto direction = ray.direction[current->Axis()];
        if (direction < 0.0) {
          work_list[queue_size++] = &current->LeftChild();
          current = &current->RightChild();
        } else {
          work_list[queue_size++] = &current->RightChild();
          current = &current->LeftChild();
        }

        continue;
      }

      auto shapes = current->Shapes();
      for (size_t i = 0; i < shapes.second; i++) {
        auto geometry = scene_objects.GetGeometry(shapes.first + i);
        intersector.Intersect(geometry.first, geometry.second);
      }
    }

    if (queue_size == 0) {
      break;
    }

    queue_size -= 1;
    current = work_list[queue_size];
  }
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris