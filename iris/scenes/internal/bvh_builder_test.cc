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
using ::testing::UnorderedElementsAre;

TEST(FindBestSplitOnAxis, Compute) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  BoundingBox::Builder bounds;
  BoundingBox::Builder centroid_bounds;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    BoundingBox bounds0(Point(0.0, i, 0.0), Point(1.0, i + 1, 1.0));
    bounds.Add(bounds0);
    centroid_bounds.Add(bounds0.Center());
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split =
      internal::FindBestSplitOnAxis(geometry_bounds, indices, bounds.Build(),
                                    centroid_bounds.Build(), Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_EQ(6.0, *best_split);
}

TEST(FindBestSplitOnAxis, TooMuchOverlap) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  BoundingBox::Builder bounds;
  BoundingBox::Builder centroid_bounds;
  for (size_t i = 0; i < internal::kNumShapesPerNode; i++) {
    BoundingBox bounds0(Point(0.0, 0.0 + 0.01 * i, 0.0),
                        Point(1.0, 1.0 + 0.01 * i, 1.0));
    bounds.Add(bounds0);
    centroid_bounds.Add(bounds0.Center());
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split =
      internal::FindBestSplitOnAxis(geometry_bounds, indices, bounds.Build(),
                                    centroid_bounds.Build(), Vector::Y_AXIS);
  ASSERT_FALSE(best_split.has_value());
}

TEST(FindBestSplitOnAxis, LotsOfOverlappedGeometry) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  BoundingBox::Builder bounds;
  BoundingBox::Builder centroid_bounds;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    bounds.Add(bounds0);
    centroid_bounds.Add(bounds0.Center());
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split =
      internal::FindBestSplitOnAxis(geometry_bounds, indices, bounds.Build(),
                                    centroid_bounds.Build(), Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_NEAR(1.0, *best_split, 0.0000001);
}

TEST(FindBestSplitOnAxis, AllOverlappedGeometry) {
  std::vector<BoundingBox> geometry_bounds;
  std::vector<size_t> indices;
  BoundingBox::Builder bounds;
  BoundingBox::Builder centroid_bounds;
  for (size_t i = 0; i < internal::kNumSplitsToEvaluate; i++) {
    geometric_t bound_offset =
        static_cast<geometric_t>(1.0) /
        static_cast<geometric_t>(internal::kNumSplitsToEvaluate);
    BoundingBox bounds0(
        Point(0.0, -0.5 + bound_offset * i + 0.5 * bound_offset, 0.0),
        Point(1.0, 99.5 + bound_offset * i + 0.5 * bound_offset, 1.0));
    bounds.Add(bounds0);
    centroid_bounds.Add(bounds0.Center());
    geometry_bounds.emplace_back(bounds0);
    indices.push_back(i);
  }

  std::optional<geometric_t> best_split =
      internal::FindBestSplitOnAxis(geometry_bounds, indices, bounds.Build(),
                                    centroid_bounds.Build(), Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_NEAR(50.0, *best_split, 0.0000001);
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
  EXPECT_FALSE(internal::TryAddLeafNode(indices, 0u, geometry_bounds, bvh, 0u,
                                        geometry_offset, geometry_sort_order));

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
  EXPECT_FALSE(internal::TryAddLeafNode(indices, internal::kMaxBvhDepth,
                                        geometry_bounds, bvh, 0u,
                                        geometry_offset, geometry_sort_order));

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
  EXPECT_FALSE(internal::TryAddLeafNode(indices, 0u, geometry_bounds, bvh, 0u,
                                        geometry_offset, geometry_sort_order));

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
  EXPECT_FALSE(internal::TryAddLeafNode(indices, 0u, geometry_bounds, bvh, 0u,
                                        geometry_offset, geometry_sort_order));

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
  std::optional<internal::WorkItem> result =
      internal::TryAddLeafNode(indices, 0u, geometry_bounds, bvh, 0u,
                               geometry_offset, geometry_sort_order);
  ASSERT_TRUE(result);

  EXPECT_THAT(result->left_indices, ElementsAre(0u));
  EXPECT_THAT(result->right_indices, ElementsAre(1u));
  EXPECT_EQ(result->parent_split_axis, Vector::Y_AXIS);
  EXPECT_EQ(result->parent_node_index, 0u);
  EXPECT_EQ(result->depth, 1u);
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
  std::optional<internal::WorkItem> result =
      internal::TryAddLeafNode(indices, 0u, geometry_bounds, bvh, 0u,
                               geometry_offset, geometry_sort_order);
  ASSERT_TRUE(result);

  EXPECT_THAT(result->left_indices, ElementsAre(1u));
  EXPECT_THAT(result->right_indices, ElementsAre(0u));
  EXPECT_EQ(result->parent_split_axis, Vector::Y_AXIS);
  EXPECT_EQ(result->parent_node_index, 0u);
  EXPECT_EQ(result->depth, 1u);
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
  for (size_t i = 0; i < 2; i++) {
    ReferenceCounted<MockGeometry> mock_geometry =
        MakeReferenceCounted<MockGeometry>();
    BoundingBox bounds0(Point(0.0, (2 - i) * 2, 0.0),
                        Point(1.0, (2 - i) * 2 + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
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

  ASSERT_EQ(2u, result.geometry_sort_order.size());
  EXPECT_EQ(1u, result.geometry_sort_order[0]);
  EXPECT_EQ(0u, result.geometry_sort_order[1]);
}

TEST(BuildBVH, FullTwoGeometryOneEmpty) {
  std::vector<std::pair<const ReferenceCounted<Geometry>, const Matrix*>>
      geometry;

  ReferenceCounted<MockGeometry> empty_geometry =
      MakeReferenceCounted<MockGeometry>();
  BoundingBox empty_bounds(Point(0.0, 0.0, 0.0));
  EXPECT_CALL(*empty_geometry, ComputeBounds(nullptr))
      .WillRepeatedly(testing::Return(empty_bounds));
  geometry.emplace_back(empty_geometry, nullptr);

  for (size_t i = 0; i < 2; i++) {
    ReferenceCounted<MockGeometry> mock_geometry =
        MakeReferenceCounted<MockGeometry>();
    BoundingBox bounds0(Point(0.0, (2 - i) * 2, 0.0),
                        Point(1.0, (2 - i) * 2 + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
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

  ASSERT_EQ(3u, result.geometry_sort_order.size());
  EXPECT_EQ(3u, result.geometry_sort_order[0]);
  EXPECT_EQ(1u, result.geometry_sort_order[1]);
  EXPECT_EQ(0u, result.geometry_sort_order[2]);
}

}  // namespace internal
}  // namespace scenes
}  // namespace iris
