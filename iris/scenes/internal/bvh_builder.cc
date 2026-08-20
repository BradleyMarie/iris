#include "iris/scenes/internal/bvh_builder.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <deque>
#include <functional>
#include <optional>
#include <span>
#include <utility>
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
namespace {

BoundingBox ComputeBounds(const std::vector<BoundingBox>& geometry_bounds,
                          std::span<const size_t> indices) {
  BoundingBox::Builder builder;
  for (size_t index : indices) {
    const BoundingBox& bounds = geometry_bounds[index];
    builder.Add(bounds.lower);
    builder.Add(bounds.upper);
  }
  return builder.Build();
}

BoundingBox ComputeCentroidBounds(
    const std::vector<BoundingBox>& geometry_bounds,
    std::span<const size_t> indices) {
  BoundingBox::Builder builder;
  for (size_t index : indices) {
    builder.Add(geometry_bounds[index].Center());
  }
  return builder.Build();
}

struct BVHSplit {
  BoundingBox::Builder bounds;
  size_t num_shapes = 0;
};

template <typename InputIterator, typename OutputIterator>
void ComputeCosts(InputIterator begin, InputIterator end,
                  OutputIterator output) {
  BoundingBox::Builder bounds_builder;
  size_t cumulative_num_shapes = 0;

  for (auto iter = begin; iter < end; ++iter) {
    cumulative_num_shapes += iter->num_shapes;
    bounds_builder.Add(iter->bounds.Build());
    *output++ = bounds_builder.Build().SurfaceArea() *
                static_cast<geometric_t>(cumulative_num_shapes);
  }
}

std::array<geometric_t, kNumSplitsToEvaluate - 1> ComputeAboveCosts(
    const std::array<BVHSplit, kNumSplitsToEvaluate>& splits) {
  std::array<geometric_t, kNumSplitsToEvaluate - 1> result;
  ComputeCosts(splits.rbegin(), splits.rend() - 1, result.rbegin());
  return result;
}

std::array<geometric_t, kNumSplitsToEvaluate - 1> ComputeBelowCosts(
    const std::array<BVHSplit, kNumSplitsToEvaluate>& splits) {
  std::array<geometric_t, kNumSplitsToEvaluate - 1> result;
  ComputeCosts(splits.begin(), splits.end() - 1, result.begin());
  return result;
}

std::array<BVHSplit, kNumSplitsToEvaluate> ComputeSplits(
    const std::vector<BoundingBox>& geometry_bounds,
    std::span<const size_t> indices, const BoundingBox& centroid_bounds,
    Vector::Axis split_axis) {
  assert(centroid_bounds.lower[split_axis] !=
         centroid_bounds.upper[split_axis]);

  geometric min = centroid_bounds.lower[split_axis];
  geometric max = centroid_bounds.upper[split_axis];
  geometric_t inv_range = static_cast<geometric>(1.0) / (max - min);

  std::array<BVHSplit, kNumSplitsToEvaluate> result;
  for (size_t index : indices) {
    const BoundingBox& bounds = geometry_bounds[index];
    geometric_t center = bounds.Center(split_axis);
    geometric_t scaled_offset = (center - min) * inv_range;

    size_t split_index =
        static_cast<geometric_t>(kNumSplitsToEvaluate) * scaled_offset;
    split_index = std::min(kNumSplitsToEvaluate - 1, split_index);

    BVHSplit& split = result[split_index];
    split.bounds.Add(bounds.lower);
    split.bounds.Add(bounds.upper);
    split.num_shapes += 1;
  }

  return result;
}

void MakeLeafNode(BVHNode& node, std::span<const size_t> indices,
                  size_t& geometry_offset,
                  std::span<size_t> geometry_sort_order) {
  node.MakeLeafNode(geometry_offset, indices.size());
  for (size_t index : indices) {
    geometry_sort_order[index] = geometry_offset++;
  }
}

struct WorkResult {
  WorkItem items[2];
  size_t num_items;
};

WorkResult ProcessWorkItem(const WorkItem& work_item,
                           const std::vector<BoundingBox>& geometry_bounds,
                           AlignedVector<BVHNode>& bvh, size_t& geometry_offset,
                           std::span<size_t> geometry_sort_order) {
  assert(!work_item.left_indices.empty());
  assert(!work_item.right_indices.empty());

  size_t left_index = bvh.size();
  size_t right_index = left_index + 1u;

  bvh[work_item.parent_node_index].MakeInteriorNode(
      work_item.parent_split_axis, left_index - work_item.parent_node_index);

  bvh.emplace_back(ComputeBounds(geometry_bounds, work_item.left_indices));
  bvh.emplace_back(ComputeBounds(geometry_bounds, work_item.right_indices));

  std::optional<WorkItem> left_work =
      TryAddLeafNode(work_item.left_indices, work_item.depth, geometry_bounds,
                     bvh, left_index, geometry_offset, geometry_sort_order);

  std::optional<WorkItem> right_work =
      TryAddLeafNode(work_item.right_indices, work_item.depth, geometry_bounds,
                     bvh, right_index, geometry_offset, geometry_sort_order);

  WorkResult result;
  result.num_items = 0u;

  if (left_work) {
    result.items[result.num_items++] = *left_work;
  }

  if (right_work) {
    result.items[result.num_items++] = *right_work;
  }

  return result;
}

void ProcessAllWorkItems(const WorkItem& work_item,
                         const std::vector<BoundingBox>& geometry_bounds,
                         AlignedVector<BVHNode>& bvh, size_t& geometry_offset,
                         std::span<size_t> geometry_sort_order) {
  constexpr size_t kBvhBfsDepthLimit = 7u;

  std::deque<WorkItem> work_list = {work_item};
  while (!work_list.empty()) {
    WorkResult work_result =
        ProcessWorkItem(work_list.front(), geometry_bounds, bvh,
                        geometry_offset, geometry_sort_order);

    if (work_result.num_items == 2) {
      geometric_t first_cost =
          bvh[work_result.items[0].parent_node_index].Bounds().SurfaceArea() *
          static_cast<geometric_t>(work_result.items[0].left_indices.size() +
                                   work_result.items[0].right_indices.size());

      geometric_t second_cost =
          bvh[work_result.items[1].parent_node_index].Bounds().SurfaceArea() *
          static_cast<geometric_t>(work_result.items[1].left_indices.size() +
                                   work_result.items[1].right_indices.size());

      if (first_cost > second_cost) {
        std::swap(work_result.items[0], work_result.items[1]);
      }
    }

    work_list.pop_front();
    for (size_t i = 0; i < work_result.num_items; i++) {
      if (work_list.empty() || work_list.front().depth < kBvhBfsDepthLimit) {
        work_list.push_back(work_result.items[i]);
      } else {
        work_list.push_front(work_result.items[i]);
      }
    }
  }
}

}  // namespace

std::optional<geometric_t> FindBestSplitOnAxis(
    const std::vector<BoundingBox>& geometry_bounds,
    std::span<const size_t> indices, const BoundingBox& node_bounds,
    const BoundingBox& centroid_bounds, Vector::Axis split_axis) {
  assert(centroid_bounds.lower[split_axis] !=
         centroid_bounds.upper[split_axis]);

  std::array<BVHSplit, kNumSplitsToEvaluate> splits =
      ComputeSplits(geometry_bounds, indices, centroid_bounds, split_axis);
  std::array<geometric_t, kNumSplitsToEvaluate - 1> below_costs =
      ComputeBelowCosts(splits);
  std::array<geometric_t, kNumSplitsToEvaluate - 1> above_costs =
      ComputeAboveCosts(splits);
  geometric_t node_surface_area = node_bounds.SurfaceArea();

  std::array<geometric_t, kNumSplitsToEvaluate - 1> costs;
  geometric_t best_cost = std::numeric_limits<geometric_t>::infinity();
  size_t num_best_costs = 1;
  for (size_t i = 0; i < costs.size(); i++) {
    costs[i] = static_cast<geometric_t>(1.0) +
               (above_costs[i] + below_costs[i]) / node_surface_area;

    if (costs[i] < best_cost) {
      best_cost = costs[i];
      num_best_costs = 1;
    } else if (costs[i] == best_cost) {
      num_best_costs += 1;
    }
  }

  if (indices.size() <= kNumShapesPerNode &&
      static_cast<geometric_t>(indices.size()) < best_cost) {
    return std::nullopt;
  }

  size_t best_split = num_best_costs / 2;
  for (size_t i = 0; i < costs.size(); i++) {
    if (costs[i] != best_cost) {
      continue;
    }

    num_best_costs -= 1;

    if (num_best_costs == 0) {
      best_split = i;
    }
  }

  return std::lerp(centroid_bounds.lower[split_axis],
                   centroid_bounds.upper[split_axis],
                   static_cast<geometric_t>(1 + best_split) /
                       static_cast<geometric_t>(kNumSplitsToEvaluate));
}

std::optional<WorkItem> TryAddLeafNode(
    const std::span<size_t> indices, const size_t depth,
    const std::vector<BoundingBox>& geometry_bounds,
    AlignedVector<BVHNode>& bvh, size_t node_index, size_t& geometry_offset,
    std::span<size_t> geometry_sort_order) {
  assert(!indices.empty());

  if (indices.size() == 1 || depth >= kMaxBvhDepth) {
    MakeLeafNode(bvh[node_index], indices, geometry_offset,
                 geometry_sort_order);
    return std::nullopt;
  }

  BoundingBox centroid_bounds = ComputeCentroidBounds(geometry_bounds, indices);
  Vector::Axis split_axis = centroid_bounds.DominantAxis();

  if (centroid_bounds.lower[split_axis] == centroid_bounds.upper[split_axis]) {
    MakeLeafNode(bvh[node_index], indices, geometry_offset,
                 geometry_sort_order);
    return std::nullopt;
  }

  std::span<size_t> above_indices, below_indices;
  if (indices.size() == 2) {
    geometric_t shape0 = geometry_bounds[indices[0]].Center(split_axis);
    geometric_t shape1 = geometry_bounds[indices[1]].Center(split_axis);

    if (shape0 < shape1) {
      below_indices = indices.subspan(0, 1);
      above_indices = indices.subspan(1, 1);
    } else {
      below_indices = indices.subspan(1, 1);
      above_indices = indices.subspan(0, 1);
    }
  } else {
    std::optional<geometric_t> split =
        FindBestSplitOnAxis(geometry_bounds, indices, bvh[node_index].Bounds(),
                            centroid_bounds, split_axis);
    if (!split.has_value()) {
      MakeLeafNode(bvh[node_index], indices, geometry_offset,
                   geometry_sort_order);
      return std::nullopt;
    }

    std::span<size_t>::iterator above_start =
        std::partition(indices.begin(), indices.end(), [&](size_t index) {
          return split < geometry_bounds[index].Center(split_axis);
        });

    // It's unclear how to write a unit test to exercise this branch; however,
    // since FindBestSplitOnAxis has been observed returning splits that do not
    // partition properly this branch is needed to cover that edge case.
    if (above_start == indices.begin() || above_start == indices.end()) {
      MakeLeafNode(bvh[node_index], indices, geometry_offset,
                   geometry_sort_order);
      return std::nullopt;
    }

    below_indices = std::span<size_t>(indices.begin(), above_start);
    above_indices = std::span<size_t>(above_start, indices.end());
  }

  return WorkItem{below_indices, above_indices, split_axis, node_index,
                  depth + 1};
}

}  // namespace internal

BuildBVHResult BuildBVH(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    size_t num_geometry, bool for_scene) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  geometry_bounds.reserve(num_geometry);
  std::vector<size_t> geometry_order;
  geometry_order.reserve(num_geometry);
  std::vector<size_t> geometry_sort_order(num_geometry, num_geometry - 1);
  for (size_t i = 0; i < num_geometry; i++) {
    auto [geometry_ref, model_to_world] = geometry(i);

    geometry_bounds.push_back(geometry_ref.ComputeBounds(model_to_world));
    if (geometry_bounds.back().Empty()) {
      continue;
    }

    world_bounds.Add(geometry_bounds.back().lower);
    world_bounds.Add(geometry_bounds.back().upper);
    geometry_order.push_back(i);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(for_scene);
  if (num_geometry != 0) {
    size_t geometry_offset = 0;
    bvh.emplace_back(world_bounds.Build());
    bvh.emplace_back(world_bounds.Build());  // Padding for cache alignment

    std::optional<internal::WorkItem> work_item =
        internal::TryAddLeafNode(geometry_order, 0u, geometry_bounds, bvh, 0u,
                                 geometry_offset, geometry_sort_order);
    if (work_item) {
      internal::ProcessAllWorkItems(*work_item, geometry_bounds, bvh,
                                    geometry_offset, geometry_sort_order);
    }
  }

  return {std::move(bvh), std::move(geometry_sort_order)};
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris
