#include "iris/scenes/internal/bvh_builder.h"

#include <array>
#include <optional>
#include <utility>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/matrix.h"
#include "iris/point.h"
#include "iris/reference_counted.h"
#include "iris/scenes/internal/aligned_vector.h"
#include "iris/scenes/internal/bvh_node.h"

namespace iris {
namespace scenes {
namespace internal {

using ::iris::geometry::MockGeometry;
using ::testing::ElementsAre;

TEST(ComputeBounds, Compute) {
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BoundingBox bounds1(Point(2.0, 2.0, 2.0), Point(3.0, 3.0, 3.0));
  BoundingBox actual_bounds =
      internal::ComputeBounds({bounds0, bounds1}, {{1, 0}});
  EXPECT_EQ(bounds0.lower, actual_bounds.lower);
  EXPECT_EQ(bounds1.upper, actual_bounds.upper);
}

TEST(ComputeCentroidBounds, Compute) {
  BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BoundingBox bounds1(Point(2.0, 2.0, 2.0), Point(3.0, 3.0, 3.0));
  BoundingBox actual_bounds =
      internal::ComputeCentroidBounds({bounds0, bounds1}, {{1, 0}});
  EXPECT_EQ(Point(0.5, 0.5, 0.5), actual_bounds.lower);
  EXPECT_EQ(Point(2.5, 2.5, 2.5), actual_bounds.upper);
}

TEST(ComputeSplits, Compute) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    BoundingBox bounds0(Point(0.0, i, 0.0), Point(1.0, i + 1, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::array<internal::BVHSplit, internal::kNumSplitsToEvaluate> splits =
      internal::ComputeSplits(
          geometry_bounds, indices,
          internal::ComputeCentroidBounds(geometry_bounds, indices),
          Vector::Y_AXIS);

  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    EXPECT_EQ(Point(0.0, i, 0.0), splits.at(i).bounds.Build().lower);
    EXPECT_EQ(Point(1.0, i + 1, 1.0), splits.at(i).bounds.Build().upper);
    EXPECT_EQ(1u, splits.at(i).num_shapes);
  }
}

TEST(ComputeAboveCosts, Compute) {
  std::array<internal::BVHSplit, internal::kNumSplitsToEvaluate> splits;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    size_t index = internal::kNumSplitsToEvaluate - i - 1;
    splits.at(index).bounds.Add(Point(index, index, index));
    splits.at(index).bounds.Add(Point(internal::kNumSplitsToEvaluate,
                                      internal::kNumSplitsToEvaluate,
                                      internal::kNumSplitsToEvaluate));
    splits.at(index).num_shapes = i + 1;
  }

  std::array<geometric_t, internal::kNumSplitsToEvaluate - 1> costs =
      internal::ComputeAboveCosts(splits);
  geometric_t cumulative_num_shapes = 0.0;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate - 1; i++) {
    size_t index = internal::kNumSplitsToEvaluate - i - 2;
    cumulative_num_shapes += static_cast<geometric_t>(i + 1);
    geometric_t surface_area = static_cast<geometric_t>((i + 1) * (i + 1) * 6);
    EXPECT_EQ(cumulative_num_shapes * surface_area, costs.at(index));
  }
}

TEST(ComputeBelowCosts, Compute) {
  std::array<internal::BVHSplit, internal::kNumSplitsToEvaluate> splits;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    splits.at(i).bounds.Add(Point(i, i, i));
    splits.at(i).bounds.Add(Point(i + 1, i + 1, i + 1));
    splits.at(i).num_shapes = i + 1;
  }

  std::array<geometric_t, internal::kNumSplitsToEvaluate - 1> costs =
      internal::ComputeBelowCosts(splits);
  geometric_t cumulative_num_shapes = 0.0;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate - 1; i++) {
    cumulative_num_shapes += static_cast<geometric_t>(i + 1);
    geometric_t surface_area = static_cast<geometric_t>((i + 1) * (i + 1) * 6);
    EXPECT_EQ(cumulative_num_shapes * surface_area, costs.at(i));
  }
}

TEST(FindBestSplitOnAxis, Compute) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    BoundingBox bounds0(Point(0.0, i, 0.0), Point(1.0, i + 1, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split = internal::FindBestSplitOnAxis(
      geometry_bounds, indices,
      internal::ComputeBounds(geometry_bounds, indices),
      internal::ComputeCentroidBounds(geometry_bounds, indices),
      Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_EQ(6.0, *best_split);
}

TEST(FindBestSplitOnAxis, TooMuchOverlap) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumShapesPerNode; i++) {
    BoundingBox bounds0(Point(0.0, 0.0 + 0.01 * i, 0.0),
                        Point(1.0, 1.0 + 0.01 * i, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split = internal::FindBestSplitOnAxis(
      geometry_bounds, indices,
      internal::ComputeBounds(geometry_bounds, indices),
      internal::ComputeCentroidBounds(geometry_bounds, indices),
      Vector::Y_AXIS);
  ASSERT_FALSE(best_split.has_value());
}

TEST(FindBestSplitOnAxis, LotsOfOverlappedGeometry) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split = internal::FindBestSplitOnAxis(
      geometry_bounds, indices,
      internal::ComputeBounds(geometry_bounds, indices),
      internal::ComputeCentroidBounds(geometry_bounds, indices),
      Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_NEAR(1.0, *best_split, 0.0000001);
}

TEST(FindBestSplitOnAxis, AllOverlappedGeometry) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, -0.5 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 99.5 + bound_offset * i + 0.5 * bound_offset, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split = internal::FindBestSplitOnAxis(
      geometry_bounds, indices,
      internal::ComputeBounds(geometry_bounds, indices),
      internal::ComputeCentroidBounds(geometry_bounds, indices),
      Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_NEAR(50.0, *best_split, 0.0000001);
}

TEST(Partition, LotsOfOverlappedGeometry) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  internal::PartitionResult result =
      internal::Partition(geometry_bounds, Vector::Y_AXIS, 1.0, indices);
  EXPECT_THAT(std::vector<size_t>(result.below.begin(), result.below.end()),
              ElementsAre(0u, 1u, 2u, 3u, 4u, 5u));
  EXPECT_THAT(std::vector<size_t>(result.above.begin(), result.above.end()),
              ElementsAre(6u, 7u, 8u, 9u, 10u, 11u));
}

TEST(MakeLeafNode, Add) {
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumShapesPerNode; i++) {
    indices.push_back(i);
  }

  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(bounds);

  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::MakeLeafNode(bvh, bvh.size() - 1, indices, geometry_offset,
                         geometry_sort_order);
  EXPECT_EQ(internal::kNumShapesPerNode, geometry_offset);
  EXPECT_EQ(indices, geometry_sort_order);

  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Geometry().first);
  EXPECT_EQ(internal::kNumShapesPerNode, bvh.front().Geometry().second);
}

TEST(BuildBVH, OneGeometry) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 1; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
    world_bounds.Add(bounds0);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(world_bounds.Build());
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::BuildBVH(bvh, 0u, geometry_bounds, 32u, indices, geometry_offset,
                     geometry_sort_order);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Geometry().first);
  EXPECT_EQ(1u, bvh.front().Geometry().second);
}

TEST(BuildBVH, DepthLimit) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
    world_bounds.Add(bounds0);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(world_bounds.Build());
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::BuildBVH(bvh, 0u, geometry_bounds, 0u, indices, geometry_offset,
                     geometry_sort_order);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Geometry().first);
  EXPECT_EQ(internal::kNumSplitsToEvaluate, bvh.front().Geometry().second);
}

TEST(BuildBVH, TooMuchOverlap) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumShapesPerNode; i++) {
    BoundingBox bounds0(Point(0.0, 0.0 + 0.01 * i, 0.0),
                        Point(1.0, 1.0 + 0.01 * i, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
    world_bounds.Add(bounds0);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(world_bounds.Build());
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::BuildBVH(bvh, 0u, geometry_bounds, 32u, indices, geometry_offset,
                     geometry_sort_order);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Geometry().first);
  EXPECT_EQ(internal::kNumShapesPerNode, bvh.front().Geometry().second);
}

TEST(BuildBVH, EmptyCentroidBounds) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    BoundingBox bounds0(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
    world_bounds.Add(bounds0);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(world_bounds.Build());
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::BuildBVH(bvh, 0u, geometry_bounds, 32u, indices, geometry_offset,
                     geometry_sort_order);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Geometry().first);
  EXPECT_EQ(internal::kNumSplitsToEvaluate, bvh.front().Geometry().second);
}

TEST(BuildBVH, TwoGeometry) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 2; i++) {
    BoundingBox bounds0(Point(0.0, i * 2, 0.0), Point(1.0, i * 2 + 1, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
    world_bounds.Add(bounds0);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(world_bounds.Build());
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::BuildBVH(bvh, 0u, geometry_bounds, 32u, indices, geometry_offset,
                     geometry_sort_order);

  EXPECT_EQ(3u, bvh.size());
  EXPECT_TRUE(bvh.at(0).HasChildren());
  EXPECT_EQ(Vector::Y_AXIS, bvh.at(0).Axis());
  const BVHNode& left_child = bvh.at(0).LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Geometry().first);
  EXPECT_EQ(1u, left_child.Geometry().second);
  const BVHNode& right_child = bvh.at(0).RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(1u, right_child.Geometry().first);
  EXPECT_EQ(1u, right_child.Geometry().second);

  EXPECT_EQ(0u, geometry_sort_order.at(0));
  EXPECT_EQ(1u, geometry_sort_order.at(1));
}

TEST(BuildBVH, TwoGeometryReversed) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 2; i++) {
    BoundingBox bounds0(Point(0.0, (2 - i) * 2, 0.0),
                        Point(1.0, (2 - i) * 2 + 1, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
    world_bounds.Add(bounds0);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(world_bounds.Build());
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::BuildBVH(bvh, 0u, geometry_bounds, 32u, indices, geometry_offset,
                     geometry_sort_order);

  EXPECT_EQ(3u, bvh.size());
  EXPECT_TRUE(bvh.at(0).HasChildren());
  EXPECT_EQ(Vector::Y_AXIS, bvh.at(0).Axis());
  const BVHNode& left_child = bvh.at(0).LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Geometry().first);
  EXPECT_EQ(1u, left_child.Geometry().second);
  const BVHNode& right_child = bvh.at(0).RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(1u, right_child.Geometry().first);
  EXPECT_EQ(1u, right_child.Geometry().second);

  EXPECT_EQ(1u, geometry_sort_order.at(0));
  EXPECT_EQ(0u, geometry_sort_order.at(1));
}

TEST(BuildBVH, HitsDepthLimit) {
  BoundingBox::Builder world_bounds;
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
    world_bounds.Add(bounds0);
  }

  AlignedVector<BVHNode> bvh = MakeAlignedVector<BVHNode>(false);
  bvh.emplace_back(world_bounds.Build());
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  internal::BuildBVH(bvh, 0u, geometry_bounds, 1u, indices, geometry_offset,
                     geometry_sort_order);

  EXPECT_EQ(3u, bvh.size());
  EXPECT_TRUE(bvh.at(0).HasChildren());
  EXPECT_EQ(Vector::Y_AXIS, bvh.at(0).Axis());
  const BVHNode& left_child = bvh.at(0).LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Geometry().first);
  EXPECT_EQ(internal::kNumSplitsToEvaluate / 2, left_child.Geometry().second);
  const BVHNode& right_child = bvh.at(0).RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(internal::kNumSplitsToEvaluate / 2, right_child.Geometry().first);
  EXPECT_EQ(internal::kNumSplitsToEvaluate / 2, right_child.Geometry().second);

  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    EXPECT_EQ(i, geometry_sort_order.at(i));
  }
}

TEST(BuildBVH, NoGeometry) {
  std::vector<std::pair<const ReferenceCounted<Geometry>, const Matrix*>>
      geometry;
  BuildBVHResult result = BuildBVH(
      [geometry](size_t index) {
        const std::pair<const ReferenceCounted<Geometry>, const Matrix*>&
            entry = geometry.at(index);
        return std::pair<const Geometry&, const Matrix*>(*entry.first,
                                                         entry.second);
      },
      geometry.size(), false);
  EXPECT_TRUE(result.bvh.empty());
  EXPECT_TRUE(result.geometry_sort_order.empty());
}

TEST(BuildBVH, FullTwoGeometry) {
  std::vector<std::pair<const ReferenceCounted<Geometry>, const Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 2; i++) {
    ReferenceCounted<MockGeometry> mock_geometry =
        MakeReferenceCounted<MockGeometry>();
    BoundingBox bounds0(Point(0.0, (2 - i) * 2, 0.0),
                        Point(1.0, (2 - i) * 2 + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  BuildBVHResult result = BuildBVH(
      [geometry](size_t index) {
        const std::pair<const ReferenceCounted<Geometry>, const Matrix*>&
            entry = geometry.at(index);
        return std::pair<const Geometry&, const Matrix*>(*entry.first,
                                                         entry.second);
      },
      geometry.size(), false);

  ASSERT_FALSE(result.bvh.empty());
  EXPECT_TRUE(result.bvh[0].HasChildren());
  EXPECT_EQ(Vector::Y_AXIS, result.bvh[0].Axis());
  const BVHNode& left_child = result.bvh[0].LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Geometry().first);
  EXPECT_EQ(1u, left_child.Geometry().second);
  const BVHNode& right_child = result.bvh[0].RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(1u, right_child.Geometry().first);
  EXPECT_EQ(1u, right_child.Geometry().second);

  EXPECT_EQ(1u, result.geometry_sort_order.at(0));
  EXPECT_EQ(0u, result.geometry_sort_order.at(1));
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris
