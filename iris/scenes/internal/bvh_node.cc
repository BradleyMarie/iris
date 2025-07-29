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

BVHNode::BVHNode(const BoundingBox& bounds)
    : bounds_(bounds),
      offset_(0u),
      num_geometry_(0u),
      axis_(std::numeric_limits<uint16_t>::max()) {}

void BVHNode::MakeInteriorNode(Vector::Axis split_axis, uint32_t child_offset) {
  assert(axis_ == std::numeric_limits<uint16_t>::max());
  offset_ = child_offset;
  num_geometry_ = 0u;
  axis_ = static_cast<uint16_t>(split_axis);
}

void BVHNode::MakeLeafNode(size_t shape_offset, size_t num_geometry) {
  assert(num_geometry != 0);
  assert(shape_offset < std::numeric_limits<uint32_t>::max());
  assert(num_geometry < std::numeric_limits<uint16_t>::max());
  assert(axis_ == std::numeric_limits<uint16_t>::max());
  offset_ = shape_offset;
  num_geometry_ = num_geometry;
  axis_ = 0u;
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris
