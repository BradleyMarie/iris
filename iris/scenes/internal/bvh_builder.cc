#include "iris/scenes/internal/bvh_builder.h"

namespace iris {
namespace scenes {
namespace internal {
namespace internal {
namespace {

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

}  // namespace

BoundingBox ComputeBounds(
    const std::pair<const Geometry&, const Matrix*>& geometry) {
  if (geometry.second) {
    return geometry.first.ComputeBounds(*geometry.second);
  }
  return geometry.first.ComputeBounds(Matrix::Identity());
}

BoundingBox ComputeBounds(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    std::span<const size_t> indices) {
  BoundingBox::Builder builder;
  for (const auto& index : indices) {
    BoundingBox bounds = geometry(index).first.ComputeBounds(
        geometry(index).second ? *geometry(index).second : Matrix::Identity());
    builder.Add(bounds);
  }
  return builder.Build();
}

Point ComputeCentroid(const BoundingBox& bounds) {
  return bounds.lower + 0.5 * (bounds.upper - bounds.lower);
}

Point ComputeCentroid(
    const std::pair<const Geometry&, const Matrix*>& geometry) {
  return ComputeCentroid(ComputeBounds(geometry));
}

BoundingBox ComputeCentroidBounds(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    std::span<const size_t> indices) {
  BoundingBox::Builder builder;
  for (const auto& index : indices) {
    Point centroid = ComputeCentroid(geometry(index));
    builder.Add(centroid);
  }
  return builder.Build();
}

std::array<BVHSplit, kNumSplitsToEvaluate> ComputeSplits(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    std::span<const size_t> indices, const BoundingBox& centroid_bounds,
    Vector::Axis split_axis) {
  assert(centroid_bounds.lower[split_axis] !=
         centroid_bounds.upper[split_axis]);

  geometric min = centroid_bounds.lower[split_axis];
  geometric max = centroid_bounds.upper[split_axis];
  geometric range = max - min;

  std::array<BVHSplit, kNumSplitsToEvaluate> result;
  for (size_t index : indices) {
    auto bounds = ComputeBounds(geometry(index));
    geometric value = ComputeCentroid(bounds)[split_axis];
    geometric_t offset = value - min;
    geometric_t scaled_offset = offset / range;

    size_t split_index =
        static_cast<geometric_t>(kNumSplitsToEvaluate) * scaled_offset;
    if (split_index == kNumSplitsToEvaluate) {
      split_index = kNumSplitsToEvaluate - 1;
    }

    result.at(split_index).bounds.Add(bounds);
    result.at(split_index).num_shapes += 1;
  }

  return result;
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

std::optional<geometric_t> FindBestSplitOnAxis(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    std::span<const size_t> indices, const BoundingBox& node_bounds,
    const BoundingBox& centroid_bounds, Vector::Axis split_axis) {
  assert(centroid_bounds.lower[split_axis] !=
         centroid_bounds.upper[split_axis]);

  auto splits = ComputeSplits(geometry, indices, centroid_bounds, split_axis);
  auto below_costs = ComputeBelowCosts(splits);
  auto above_costs = ComputeAboveCosts(splits);
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

PartitionResult Partition(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    Vector::Axis split_axis, geometric_t split, std::span<size_t> indices) {
  size_t insert_index = 0;

  for (size_t i = 0; i < indices.size(); i++) {
    auto centroid = ComputeCentroid(geometry(indices[i]));
    auto value = centroid[split_axis];
    if (value < split) {
      std::swap(indices[i], indices[insert_index++]);
    }
  }

  return {indices.subspan(insert_index, indices.size() - insert_index),
          indices.subspan(0, insert_index)};
}

size_t AddLeafNode(std::span<const size_t> indices,
                   const BoundingBox& node_bounds, std::vector<BVHNode>& bvh,
                   size_t& geometry_offset,
                   std::span<size_t> geometry_sort_order) {
  bvh.push_back(
      BVHNode::MakeLeafNode(node_bounds, geometry_offset, indices.size()));
  for (size_t index : indices) {
    geometry_sort_order[index] = geometry_offset++;
  }
  return bvh.size() - 1;
}

size_t AddInteriorNode(const BoundingBox& node_bounds, Vector::Axis split_axis,
                       std::vector<BVHNode>& bvh) {
  bvh.push_back(BVHNode::MakeInteriorNode(node_bounds, split_axis));
  return bvh.size() - 1;
}

size_t BuildBVH(const std::function<std::pair<const Geometry&, const Matrix*>(
                    size_t index)>& geometry,
                size_t depth_remaining, std::span<size_t> indices,
                std::vector<BVHNode>& bvh, size_t& geometry_offset,
                std::span<size_t> geometry_sort_order) {
  assert(!indices.empty());

  auto node_bounds = ComputeBounds(geometry, indices);
  if (indices.size() == 1 || depth_remaining == 0) {
    return AddLeafNode(indices, node_bounds, bvh, geometry_offset,
                       geometry_sort_order);
  }

  auto centroid_bounds = ComputeCentroidBounds(geometry, indices);
  auto centroid_bounds_diagonal = centroid_bounds.upper - centroid_bounds.lower;
  auto split_axis = centroid_bounds_diagonal.DominantAxis();

  if (centroid_bounds.lower[split_axis] == centroid_bounds.upper[split_axis]) {
    return AddLeafNode(indices, node_bounds, bvh, geometry_offset,
                       geometry_sort_order);
  }

  std::span<size_t> above_indices, below_indices;
  if (indices.size() == 2) {
    geometric_t shape0 = ComputeCentroid(geometry(indices[0]))[split_axis];
    geometric_t shape1 = ComputeCentroid(geometry(indices[1]))[split_axis];

    if (shape0 < shape1) {
      below_indices = indices.subspan(0, 1);
      above_indices = indices.subspan(1, 1);
    } else {
      below_indices = indices.subspan(1, 1);
      above_indices = indices.subspan(0, 1);
    }
  } else {
    auto split = FindBestSplitOnAxis(geometry, indices, node_bounds,
                                     centroid_bounds, split_axis);
    if (!split.has_value()) {
      return AddLeafNode(indices, node_bounds, bvh, geometry_offset,
                         geometry_sort_order);
    }

    auto result = Partition(geometry, split_axis, *split, indices);
    if (result.above.empty() || result.below.empty()) {
      // It's unclear how to write a unit test to exercise this branch; however,
      // since FindBestSplitOnAxis has been observed returning splits that do
      // not partition properly this branch is needed to cover that edge case.
      return AddLeafNode(indices, node_bounds, bvh, geometry_offset,
                         geometry_sort_order);
    }

    below_indices = result.below;
    above_indices = result.above;
  }

  size_t result = AddInteriorNode(node_bounds, split_axis, bvh);
  BuildBVH(geometry, depth_remaining - 1, below_indices, bvh, geometry_offset,
           geometry_sort_order);
  size_t right_child = BuildBVH(geometry, depth_remaining - 1, above_indices,
                                bvh, geometry_offset, geometry_sort_order);
  bvh.at(result).SetRightChildOffset(right_child - result);

  return result;
}

}  // namespace internal

BuildBVHResult BuildBVH(
    const std::function<std::pair<const Geometry&, const Matrix*>(size_t)>&
        geometry,
    size_t num_geometry) {
  std::vector<size_t> geometry_order;
  std::vector<size_t> geometry_sort_order;
  for (size_t i = 0; i < num_geometry; i++) {
    geometry_order.push_back(i);
    geometry_sort_order.push_back(num_geometry);
  }

  std::vector<BVHNode> bvh;
  if (num_geometry != 0) {
    size_t geometry_offset = 0;
    internal::BuildBVH(geometry, internal::kMaxBvhDepth, geometry_order, bvh,
                       geometry_offset, geometry_sort_order);
    bvh.shrink_to_fit();
  }

  return {std::move(bvh), std::move(geometry_sort_order)};
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris