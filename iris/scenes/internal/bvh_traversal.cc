#include "iris/scenes/internal/bvh_traversal.h"

#include <limits>

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
    bool intersects = current->Intersects(ray, intersector.MinimumDistance(),
                                          intersector.MaximumDistance());
    if (intersects) {
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

Hit* Intersect(const BVHNode& bvh,
               const std::vector<ReferenceCounted<Geometry>>& geometry,
               const Ray& ray, HitAllocator& hit_allocator) {
  geometric_t minimum_distance = 0.0;
  geometric_t maximum_distance = std::numeric_limits<geometric_t>::infinity();

  const BVHNode* work_list[internal::kMaxBvhDepth];
  const BVHNode* current = &bvh;
  size_t queue_size = 0;
  Hit* closest_hit = nullptr;
  for (;;) {
    bool intersects =
        current->Intersects(ray, minimum_distance, maximum_distance);
    if (intersects) {
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
        for (auto* hit_list = geometry[shapes.first + i]->Trace(hit_allocator);
             hit_list; hit_list = hit_list->next) {
          if (hit_list->distance > minimum_distance &&
              hit_list->distance < maximum_distance) {
            closest_hit = hit_list;
            maximum_distance = hit_list->distance;
          }
        }
      }
    }

    if (queue_size == 0) {
      break;
    }

    queue_size -= 1;
    current = work_list[queue_size];
  }

  return closest_hit;
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris