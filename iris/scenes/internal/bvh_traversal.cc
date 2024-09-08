#include "iris/scenes/internal/bvh_traversal.h"

#include <limits>

#include "iris/scenes/internal/bvh_builder.h"

namespace iris {
namespace scenes {
namespace internal {
namespace {

class Intersector {
 public:
  Intersector(geometric_t min_dist, geometric_t max_dist)
      : minimum_distance(min_dist), maximum_distance(max_dist) {}

  virtual void Intersect(const BVHNode& bvh_node) = 0;

  geometric_t minimum_distance;
  geometric_t maximum_distance;
};

class GeometryIntersector : public Intersector {
 public:
  GeometryIntersector(const SceneObjects& scene_objects,
                      iris::Intersector& intersector)
      : Intersector(intersector.MinimumDistance(),
                    intersector.MaximumDistance()),
        scene_objects_(scene_objects),
        intersector_(intersector) {}

  void Intersect(const BVHNode& bvh_node) override {
    auto [start_index, num_shapes] = bvh_node.Shapes();
    for (size_t i = 0; i < num_shapes; i++) {
      auto [geometry, model_to_world] =
          scene_objects_.GetGeometry(start_index + i);
      intersector_.Intersect(geometry, model_to_world);
    }
    minimum_distance = intersector_.MinimumDistance();
    maximum_distance = intersector_.MaximumDistance();
  }

 private:
  const SceneObjects& scene_objects_;
  iris::Intersector& intersector_;
};

class NestedGeometryIntersector : public Intersector {
 public:
  NestedGeometryIntersector(
      const std::vector<ReferenceCounted<Geometry>>& geometry,
      HitAllocator& hit_allocator)
      : Intersector(static_cast<geometric_t>(0.0),
                    std::numeric_limits<geometric_t>::infinity()),
        geometry_(geometry),
        hit_allocator_(hit_allocator) {}

  void Intersect(const BVHNode& bvh_node) override {
    auto [start_index, num_shapes] = bvh_node.Shapes();
    for (size_t i = 0; i < num_shapes; i++) {
      for (Hit* hit_list = geometry_[start_index + i]->Trace(hit_allocator_);
           hit_list; hit_list = hit_list->next) {
        if (hit_list->distance > minimum_distance &&
            hit_list->distance < maximum_distance) {
          closest_hit = hit_list;
          maximum_distance = hit_list->distance;
        }
      }
    }
  }

  Hit* closest_hit = nullptr;

 private:
  const std::vector<ReferenceCounted<Geometry>>& geometry_;
  HitAllocator& hit_allocator_;
};

void Intersect(const BVHNode& bvh, const Ray& ray, Intersector& intersector) {
  const BVHNode* work_list[internal::kMaxBvhDepth];
  const BVHNode* current = &bvh;
  size_t queue_size = 0;
  for (;;) {
    bool intersects = current->Intersects(ray, intersector.minimum_distance,
                                          intersector.maximum_distance);
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

      intersector.Intersect(*current);
    }

    if (queue_size == 0) {
      break;
    }

    queue_size -= 1;
    current = work_list[queue_size];
  }
}

}  // namespace

void Intersect(const BVHNode& bvh, const SceneObjects& scene_objects,
               const Ray& ray, iris::Intersector& intersector) {
  GeometryIntersector nested_intersector(scene_objects, intersector);
  Intersect(bvh, ray, nested_intersector);
}

Hit* Intersect(const BVHNode& bvh,
               const std::vector<ReferenceCounted<Geometry>>& geometry,
               const Ray& ray, HitAllocator& hit_allocator) {
  NestedGeometryIntersector nested_intersector(geometry, hit_allocator);
  Intersect(bvh, ray, nested_intersector);
  return nested_intersector.closest_hit;
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris