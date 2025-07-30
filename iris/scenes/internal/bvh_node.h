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
  BVHNode(const BoundingBox& bounds);
  BVHNode(const BVHNode& node) = default;

  void MakeInteriorNode(Vector::Axis split_axis, size_t child_offset);
  void MakeLeafNode(size_t geometry_offset, size_t num_geometry);

  const BoundingBox& Bounds() const { return bounds_; }

  inline bool Intersects(const Ray& ray, geometric_t minimum_distance,
                         geometric_t maximum_distance) const {
    return bounds_.Intersects(ray, minimum_distance, maximum_distance);
  }

  inline bool HasChildren() const { return num_geometry_ == 0; }

  inline Vector::Axis Axis() const {
    assert(num_geometry_ == 0);
    return Vector::Axis(axis_);
  }

  inline const BVHNode& LeftChild() const {
    assert(num_geometry_ == 0);
    return *(this + offset_);
  }

  inline const BVHNode& RightChild() const {
    assert(num_geometry_ == 0);
    return *(this + offset_ + 1);
  }

  inline std::pair<size_t, size_t> Geometry() const {
    assert(num_geometry_ != 0);
    return std::pair<size_t, size_t>(offset_, num_geometry_);
  }

 private:
  BoundingBox bounds_;
  uint32_t offset_;
  uint16_t num_geometry_;
  uint16_t axis_;
};

}  // namespace internal
}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_INTERNAL_BVH_NODE_
