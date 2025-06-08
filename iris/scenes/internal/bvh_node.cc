#undef NDEBUG  // Enable assertions at runtime
#include "iris/scenes/internal/bvh_node.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <utility>

#include "iris/bounding_box.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace scenes {
namespace internal {

BVHNode BVHNode::MakeInteriorNode(const BoundingBox& bounds,
                                  Vector::Axis split_axis) {
  return BVHNode(bounds, /*offset=*/0u, /*num_shapes=*/0u,
                 /*axis=*/static_cast<uint16_t>(split_axis));
}

BVHNode BVHNode::MakeLeafNode(const BoundingBox& bounds, size_t shape_offset,
                              size_t num_shapes) {
  assert(num_shapes != 0);
  assert(shape_offset < std::numeric_limits<uint32_t>::max());
  assert(num_shapes < std::numeric_limits<uint16_t>::max());
  return BVHNode(bounds, /*offset=*/static_cast<uint32_t>(shape_offset),
                 /*num_shapes=*/static_cast<uint16_t>(num_shapes),
                 /*axis=*/0u);
}

void BVHNode::SetRightChildOffset(size_t offset) {
  assert(offset_ == 0);
  assert(num_shapes_ == 0);
  assert(offset < std::numeric_limits<uint32_t>::max());
  offset_ = static_cast<uint32_t>(offset);
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris
