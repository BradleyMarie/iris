#ifndef _IRIS_SCENES_INTERNAL_BVH_BUILDER_
#define _IRIS_SCENES_INTERNAL_BVH_BUILDER_

#include <array>
#include <functional>
#include <optional>
#include <span>
#include <vector>

#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/matrix.h"
#include "iris/reference_counted.h"
#include "iris/scenes/internal/aligned_vector.h"
#include "iris/scenes/internal/bvh_node.h"
#include "iris/vector.h"

namespace iris {
namespace scenes {
namespace internal {
namespace internal {

constexpr size_t kNumShapesPerNode = 4;
constexpr size_t kNumSplitsToEvaluate = 12;
constexpr size_t kMaxBvhDepth = 32;

std::optional<geometric_t> FindBestSplitOnAxis(
    const std::vector<BoundingBox>& geometry_bounds,
    std::span<const size_t> indices, const BoundingBox& node_bounds,
    const BoundingBox& centroid_bounds, Vector::Axis split_axis);

struct WorkItem {
  std::span<size_t> left_indices;
  std::span<size_t> right_indices;
  Vector::Axis parent_split_axis;
  size_t parent_node_index;
  size_t depth;
};

std::optional<WorkItem> TryAddLeafNode(
    const std::span<size_t> indices, const size_t depth,
    const std::vector<BoundingBox>& geometry_bounds,
    AlignedVector<BVHNode>& bvh, size_t node_index, size_t& geometry_offset,
    std::span<size_t> geometry_sort_order);

}  // namespace internal

struct BuildBVHResult final {
  AlignedVector<BVHNode> bvh;
  std::vector<size_t> geometry_sort_order;
};

BuildBVHResult BuildBVH(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    size_t num_geometry, bool for_scene);

}  // namespace internal
}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_INTERNAL_BVH_BUILDER_
