#ifndef _IRIS_SCENES_INTERNAL_BVH_NODE_
#define _IRIS_SCENES_INTERNAL_BVH_NODE_

#include <cassert>
#include <cstdint>
#include <utility>

#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace scenes {
namespace internal {

class BVHNode final {
 public:
  BVHNode(const BVHNode& node) = default;

  static BVHNode MakeInteriorNode(const BoundingBox& bounds,
                                  Vector::Axis split_axis);
  static BVHNode MakeLeafNode(const BoundingBox& bounds, size_t shape_offset,
                              size_t num_shapes);
  void SetRightChildOffset(size_t offset);

  const BoundingBox& Bounds() const { return bounds_; }

  inline bool Intersects(const Ray& ray, geometric_t minimum_distance,
                         geometric_t maximum_distance) const {
    return bounds_.Intersects(ray, minimum_distance, maximum_distance);
  }

  inline bool HasChildren() const { return num_shapes_ == 0; }

  inline Vector::Axis Axis() const {
    assert(num_shapes_ == 0);
    return Vector::Axis(axis_);
  }

  inline const BVHNode& LeftChild() const {
    assert(num_shapes_ == 0);
    return *(this + 1);
  }

  inline const BVHNode& RightChild() const {
    assert(num_shapes_ == 0);
    return *(this + offset_);
  }

  inline std::pair<size_t, size_t> Shapes() const {
    assert(num_shapes_ != 0);
    return std::pair<size_t, size_t>(offset_, num_shapes_);
  }

 private:
  BVHNode(const BoundingBox& bounds, uint32_t offset, uint16_t num_shapes,
          uint16_t axis)
      : bounds_(bounds),
        offset_(offset),
        num_shapes_(num_shapes),
        axis_(axis) {}

  BoundingBox bounds_;
  uint32_t offset_;
  uint16_t num_shapes_;
  uint16_t axis_;
};

}  // namespace internal
}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_INTERNAL_BVH_NODE_
