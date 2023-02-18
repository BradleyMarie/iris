#ifndef _IRIS_SCENES_INTERNAL_BVH_BUILDER_
#define _IRIS_SCENES_INTERNAL_BVH_BUILDER_

#include <array>
#include <optional>
#include <span>

#include "iris/geometry.h"
#include "iris/reference_counted.h"
#include "iris/scenes/internal/bvh_node.h"

namespace iris {
namespace scenes {
namespace internal {
namespace internal {

constexpr size_t kNumShapesPerNode = 4;
constexpr size_t kNumSplitsToEvaluate = 12;
constexpr size_t kMaxBvhDepth = 32;

BoundingBox ComputeBounds(
    const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>
        geometry);

BoundingBox ComputeBounds(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry,
    std::span<const size_t> indices);

Point ComputeCentroid(const BoundingBox& bounds);

Point ComputeCentroid(const std::pair<const iris::ReferenceCounted<Geometry>,
                                      const Matrix*>& geometry);

BoundingBox ComputeCentroidBounds(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry,
    std::span<const size_t> indices);

struct BVHSplit {
  BoundingBox::Builder bounds;
  size_t num_shapes = 0;
};

std::optional<std::array<BVHSplit, kNumSplitsToEvaluate>> ComputeSplits(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry,
    std::span<const size_t> indices, const BoundingBox& centroid_bounds,
    Vector::Axis split_axis);

std::array<geometric_t, kNumSplitsToEvaluate - 1> ComputeAboveCosts(
    std::array<BVHSplit, kNumSplitsToEvaluate> splits);

std::array<geometric_t, kNumSplitsToEvaluate - 1> ComputeBelowCosts(
    std::array<BVHSplit, kNumSplitsToEvaluate> splits);

std::optional<geometric_t> FindBestSplitOnAxis(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry,
    std::span<const size_t> indices, const BoundingBox& node_bounds,
    Vector::Axis split_axis);

struct PartitionResult {
  std::span<size_t> above;
  std::span<size_t> below;
};

PartitionResult Partition(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry,
    Vector::Axis split_axis, geometric_t split, std::span<size_t> indices);

size_t AddLeafNode(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry,
    std::span<const size_t> indices, const BoundingBox& node_bounds,
    std::vector<BVHNode>* bvh, size_t* geometry_offset,
    std::span<size_t> geometry_sort_order);

size_t AddInteriorNode(const BoundingBox& node_bounds, Vector::Axis split_axis,
                       std::vector<BVHNode>* bvh);

size_t BuildBVH(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry,
    uint32_t depth_remaining, std::span<size_t> indices,
    std::vector<BVHNode>* bvh, size_t* geometry_offset,
    std::span<size_t> geometry_sort_order);

};  // namespace internal

struct BuildBVHResult {
  std::vector<BVHNode> bvh;
  std::vector<size_t> geometry_sort_order;
};

BuildBVHResult BuildBVH(
    std::span<
        const std::pair<const iris::ReferenceCounted<Geometry>, const Matrix*>>
        geometry);

}  // namespace internal
}  // namespace scenes
}  // namespace iris

#endif  // _IRIS_SCENES_INTERNAL_BVH_BUILDER_