#include "iris/scenes/internal/bvh_builder.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/reference_counted.h"

std::function<std::pair<const iris::Geometry&, const iris::Matrix*>(size_t)>
WrapGeometry(std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                                   const iris::Matrix*>>
                 geometry) {
  return [geometry](size_t index) {
    const auto& entry = geometry.at(index);
    return std::pair<const iris::Geometry&, const iris::Matrix*>(*entry.first,
                                                                 entry.second);
  };
}

TEST(ComputeBounds, NoMatrix) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(bounds));
  auto actual_bounds =
      iris::scenes::internal::internal::ComputeBounds({*geometry, nullptr});
  EXPECT_EQ(bounds.lower, actual_bounds.lower);
  EXPECT_EQ(bounds.upper, actual_bounds.upper);
}

TEST(ComputeBounds, WithMatrix) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto transformation = iris::Matrix::Translation(2.0, 2.0, 2.0).value();
  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(&transformation))
      .WillOnce(testing::Return(bounds));
  auto actual_bounds = iris::scenes::internal::internal::ComputeBounds(
      {*geometry, &transformation});
  EXPECT_EQ(bounds.lower, actual_bounds.lower);
  EXPECT_EQ(bounds.upper, actual_bounds.upper);
}

TEST(ComputeBounds, Many) {
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(1.0, 1.0, 1.0));
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 2.0),
                            iris::Point(3.0, 3.0, 3.0));
  auto geometry0 = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  auto geometry1 = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  auto geometry2 = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry0, ComputeBounds(nullptr))
      .WillOnce(testing::Return(bounds0));
  EXPECT_CALL(*geometry1, ComputeBounds(nullptr))
      .WillOnce(testing::Return(bounds1));
  EXPECT_CALL(*geometry2, ComputeBounds(nullptr)).Times(0);
  auto actual_bounds = iris::scenes::internal::internal::ComputeBounds(
      WrapGeometry(
          {{{geometry0, nullptr}, {geometry1, nullptr}, {geometry2, nullptr}}}),
      {{1, 0}});
  EXPECT_EQ(bounds0.lower, actual_bounds.lower);
  EXPECT_EQ(bounds1.upper, actual_bounds.upper);
}

TEST(ComputeCentroid, FromGeometry) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(bounds));
  auto centroid = iris::scenes::internal::internal::ComputeCentroid(
      std::pair<const iris::Geometry&, const iris::Matrix*>(*geometry,
                                                            nullptr));
  EXPECT_EQ(iris::Point(0.5, 0.5, 0.5), centroid);
}

TEST(ComputeCentroidBounds, Compute) {
  iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                            iris::Point(1.0, 1.0, 1.0));
  iris::BoundingBox bounds1(iris::Point(2.0, 2.0, 2.0),
                            iris::Point(3.0, 3.0, 3.0));
  auto geometry0 = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  auto geometry1 = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  auto geometry2 = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry0, ComputeBounds(nullptr))
      .WillOnce(testing::Return(bounds0));
  EXPECT_CALL(*geometry1, ComputeBounds(nullptr))
      .WillOnce(testing::Return(bounds1));
  EXPECT_CALL(*geometry2, ComputeBounds(nullptr)).Times(0);
  auto actual_bounds = iris::scenes::internal::internal::ComputeCentroidBounds(
      WrapGeometry(
          {{{geometry0, nullptr}, {geometry1, nullptr}, {geometry2, nullptr}}}),
      {{1, 0}});
  EXPECT_EQ(iris::Point(0.5, 0.5, 0.5), actual_bounds.lower);
  EXPECT_EQ(iris::Point(2.5, 2.5, 2.5), actual_bounds.upper);
}

TEST(ComputeSplits, Compute) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, i, 0.0),
                              iris::Point(1.0, i + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr)).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto splits = iris::scenes::internal::internal::ComputeSplits(
      WrapGeometry(geometry), indices,
      iris::scenes::internal::internal::ComputeCentroidBounds(
          WrapGeometry(geometry), indices),
      iris::Vector::Y_AXIS);

  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    EXPECT_EQ(iris::Point(0.0, i, 0.0), splits.at(i).bounds.Build().lower);
    EXPECT_EQ(iris::Point(1.0, i + 1, 1.0), splits.at(i).bounds.Build().upper);
    EXPECT_EQ(1u, splits.at(i).num_shapes);
  }
}

TEST(ComputeAboveCosts, Compute) {
  std::array<iris::scenes::internal::internal::BVHSplit,
             iris::scenes::internal::internal::kNumSplitsToEvaluate>
      splits;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    size_t index =
        iris::scenes::internal::internal::kNumSplitsToEvaluate - i - 1;
    splits.at(index).bounds.Add(iris::Point(index, index, index));
    splits.at(index).bounds.Add(
        iris::Point(iris::scenes::internal::internal::kNumSplitsToEvaluate,
                    iris::scenes::internal::internal::kNumSplitsToEvaluate,
                    iris::scenes::internal::internal::kNumSplitsToEvaluate));
    splits.at(index).num_shapes = i + 1;
  }

  auto costs = iris::scenes::internal::internal::ComputeAboveCosts(splits);
  iris::geometric_t cumulative_num_shapes = 0.0;
  for (size_t i = 0;
       i < iris::scenes::internal::internal::kNumSplitsToEvaluate - 1; i++) {
    size_t index =
        iris::scenes::internal::internal::kNumSplitsToEvaluate - i - 2;
    cumulative_num_shapes += static_cast<iris::geometric_t>(i + 1);
    auto surface_area = static_cast<iris::geometric_t>((i + 1) * (i + 1) * 6);
    EXPECT_EQ(cumulative_num_shapes * surface_area, costs.at(index));
  }
}

TEST(ComputeBelowCosts, Compute) {
  std::array<iris::scenes::internal::internal::BVHSplit,
             iris::scenes::internal::internal::kNumSplitsToEvaluate>
      splits;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    splits.at(i).bounds.Add(iris::Point(i, i, i));
    splits.at(i).bounds.Add(iris::Point(i + 1, i + 1, i + 1));
    splits.at(i).num_shapes = i + 1;
  }

  auto costs = iris::scenes::internal::internal::ComputeBelowCosts(splits);
  iris::geometric_t cumulative_num_shapes = 0.0;
  for (size_t i = 0;
       i < iris::scenes::internal::internal::kNumSplitsToEvaluate - 1; i++) {
    cumulative_num_shapes += static_cast<iris::geometric_t>(i + 1);
    auto surface_area = static_cast<iris::geometric_t>((i + 1) * (i + 1) * 6);
    EXPECT_EQ(cumulative_num_shapes * surface_area, costs.at(i));
  }
}

TEST(FindBestSplitOnAxis, Compute) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, i, 0.0),
                              iris::Point(1.0, i + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr)).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      WrapGeometry(geometry), indices,
      iris::scenes::internal::internal::ComputeBounds(WrapGeometry(geometry),
                                                      indices),
      iris::scenes::internal::internal::ComputeCentroidBounds(
          WrapGeometry(geometry), indices),
      iris::Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_EQ(6.0, *best_split);
}

TEST(FindBestSplitOnAxis, TooMuchOverlap) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumShapesPerNode;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, 0.0 + 0.01 * i, 0.0),
                              iris::Point(1.0, 1.0 + 0.01 * i, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr)).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      WrapGeometry(geometry), indices,
      iris::scenes::internal::internal::ComputeBounds(WrapGeometry(geometry),
                                                      indices),
      iris::scenes::internal::internal::ComputeCentroidBounds(
          WrapGeometry(geometry), indices),
      iris::Vector::Y_AXIS);
  ASSERT_FALSE(best_split.has_value());
}

TEST(FindBestSplitOnAxis, LotsOfOverlappedGeometry) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    auto bound_offset =
        1.0 / static_cast<iris::geometric_t>(
                  iris::scenes::internal::internal::kNumSplitsToEvaluate);
    iris::BoundingBox bounds0(
        iris::Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        iris::Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr)).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      WrapGeometry(geometry), indices,
      iris::scenes::internal::internal::ComputeBounds(WrapGeometry(geometry),
                                                      indices),
      iris::scenes::internal::internal::ComputeCentroidBounds(
          WrapGeometry(geometry), indices),
      iris::Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_NEAR(1.0, *best_split, 0.0000001);
}

TEST(FindBestSplitOnAxis, AllOverlappedGeometry) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    auto bound_offset =
        1.0 / static_cast<iris::geometric_t>(
                  iris::scenes::internal::internal::kNumSplitsToEvaluate);
    iris::BoundingBox bounds0(
        iris::Point(0.0, -0.5 + bound_offset * i + 0.5 * bound_offset, 0.0),
        iris::Point(1.0, 99.5 + bound_offset * i + 0.5 * bound_offset, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr)).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      WrapGeometry(geometry), indices,
      iris::scenes::internal::internal::ComputeBounds(WrapGeometry(geometry),
                                                      indices),
      iris::scenes::internal::internal::ComputeCentroidBounds(
          WrapGeometry(geometry), indices),
      iris::Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_NEAR(50.0, *best_split, 0.0000001);
}

TEST(Partition, LotsOfOverlappedGeometry) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    auto bound_offset =
        1.0 / static_cast<iris::geometric_t>(
                  iris::scenes::internal::internal::kNumSplitsToEvaluate);
    iris::BoundingBox bounds0(
        iris::Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        iris::Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto result = iris::scenes::internal::internal::Partition(
      WrapGeometry(geometry), iris::Vector::Y_AXIS, 1.0, indices);
  EXPECT_THAT(std::vector<size_t>(result.below.begin(), result.below.end()),
              testing::ElementsAre(0u, 1u, 2u, 3u, 4u, 5u));
  EXPECT_THAT(std::vector<size_t>(result.above.begin(), result.above.end()),
              testing::ElementsAre(6u, 7u, 8u, 9u, 10u, 11u));
}

TEST(AddLeafNode, Add) {
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumShapesPerNode;
       i++) {
    indices.push_back(i);
  }

  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::AddLeafNode(
      indices, bounds, bvh, geometry_offset, geometry_sort_order);
  EXPECT_EQ(0u, result);
  EXPECT_EQ(iris::scenes::internal::internal::kNumShapesPerNode,
            geometry_offset);
  EXPECT_EQ(indices, geometry_sort_order);
  EXPECT_EQ(1u, bvh.size());

  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Shapes().first);
  EXPECT_EQ(iris::scenes::internal::internal::kNumShapesPerNode,
            bvh.front().Shapes().second);
}

TEST(AddInteriorNode, Add) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t result = iris::scenes::internal::internal::AddInteriorNode(
      bounds, iris::Vector::Y_AXIS, bvh);
  EXPECT_EQ(0u, result);
  EXPECT_EQ(1u, bvh.size());
  EXPECT_TRUE(bvh.front().HasChildren());
  EXPECT_EQ(iris::Vector::Y_AXIS, bvh.front().Axis());
}

TEST(BuildBVH, OneGeometry) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 1; i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    auto bound_offset =
        1.0 / static_cast<iris::geometric_t>(
                  iris::scenes::internal::internal::kNumSplitsToEvaluate);
    iris::BoundingBox bounds0(
        iris::Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        iris::Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::BuildBVH(
      WrapGeometry(geometry), 32u, indices, bvh, geometry_offset,
      geometry_sort_order);
  EXPECT_EQ(0u, result);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Shapes().first);
  EXPECT_EQ(1u, bvh.front().Shapes().second);
}

TEST(BuildBVH, DepthLimit) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    auto bound_offset =
        1.0 / static_cast<iris::geometric_t>(
                  iris::scenes::internal::internal::kNumSplitsToEvaluate);
    iris::BoundingBox bounds0(
        iris::Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        iris::Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::BuildBVH(
      WrapGeometry(geometry), 0u, indices, bvh, geometry_offset,
      geometry_sort_order);
  EXPECT_EQ(0u, result);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Shapes().first);
  EXPECT_EQ(iris::scenes::internal::internal::kNumSplitsToEvaluate,
            bvh.front().Shapes().second);
}

TEST(BuildBVH, TooMuchOverlap) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumShapesPerNode;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, 0.0 + 0.01 * i, 0.0),
                              iris::Point(1.0, 1.0 + 0.01 * i, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::BuildBVH(
      WrapGeometry(geometry), 32u, indices, bvh, geometry_offset,
      geometry_sort_order);
  EXPECT_EQ(0u, result);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Shapes().first);
  EXPECT_EQ(iris::scenes::internal::internal::kNumShapesPerNode,
            bvh.front().Shapes().second);
}

TEST(BuildBVH, EmptyCentroidBounds) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                              iris::Point(1.0, 1.0, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::BuildBVH(
      WrapGeometry(geometry), 32u, indices, bvh, geometry_offset,
      geometry_sort_order);
  EXPECT_EQ(0u, result);

  EXPECT_EQ(1u, bvh.size());
  EXPECT_FALSE(bvh.front().HasChildren());
  EXPECT_EQ(0u, bvh.front().Shapes().first);
  EXPECT_EQ(iris::scenes::internal::internal::kNumSplitsToEvaluate,
            bvh.front().Shapes().second);
}

TEST(BuildBVH, TwoGeometry) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 2; i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, i * 2, 0.0),
                              iris::Point(1.0, i * 2 + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::BuildBVH(
      WrapGeometry(geometry), 32u, indices, bvh, geometry_offset,
      geometry_sort_order);
  EXPECT_EQ(0u, result);

  EXPECT_EQ(3u, bvh.size());
  EXPECT_TRUE(bvh.at(0).HasChildren());
  EXPECT_EQ(iris::Vector::Y_AXIS, bvh.at(0).Axis());
  auto& left_child = bvh.at(0).LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Shapes().first);
  EXPECT_EQ(1u, left_child.Shapes().second);
  auto& right_child = bvh.at(0).RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(1u, right_child.Shapes().first);
  EXPECT_EQ(1u, right_child.Shapes().second);

  EXPECT_EQ(0u, geometry_sort_order.at(0));
  EXPECT_EQ(1u, geometry_sort_order.at(1));
}

TEST(BuildBVH, TwoGeometryReversed) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 2; i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, (2 - i) * 2, 0.0),
                              iris::Point(1.0, (2 - i) * 2 + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::BuildBVH(
      WrapGeometry(geometry), 32u, indices, bvh, geometry_offset,
      geometry_sort_order);
  EXPECT_EQ(0u, result);

  EXPECT_EQ(3u, bvh.size());
  EXPECT_TRUE(bvh.at(0).HasChildren());
  EXPECT_EQ(iris::Vector::Y_AXIS, bvh.at(0).Axis());
  auto& left_child = bvh.at(0).LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Shapes().first);
  EXPECT_EQ(1u, left_child.Shapes().second);
  auto& right_child = bvh.at(0).RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(1u, right_child.Shapes().first);
  EXPECT_EQ(1u, right_child.Shapes().second);

  EXPECT_EQ(1u, geometry_sort_order.at(0));
  EXPECT_EQ(0u, geometry_sort_order.at(1));
}

TEST(BuildBVH, HitsDepthLimit) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    auto bound_offset =
        1.0 / static_cast<iris::geometric_t>(
                  iris::scenes::internal::internal::kNumSplitsToEvaluate);
    iris::BoundingBox bounds0(
        iris::Point(0.0, 0.0 + bound_offset * i + 0.5 * bound_offset, 0.0),
        iris::Point(1.0, 1.0 + bound_offset * i + 0.5 * bound_offset, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  std::vector<iris::scenes::internal::BVHNode> bvh;
  size_t geometry_offset = 0;
  std::vector<size_t> geometry_sort_order(indices.size(), indices.size());
  size_t result = iris::scenes::internal::internal::BuildBVH(
      WrapGeometry(geometry), 1u, indices, bvh, geometry_offset,
      geometry_sort_order);
  EXPECT_EQ(0u, result);

  EXPECT_EQ(3u, bvh.size());
  EXPECT_TRUE(bvh.at(0).HasChildren());
  EXPECT_EQ(iris::Vector::Y_AXIS, bvh.at(0).Axis());
  auto& left_child = bvh.at(0).LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Shapes().first);
  EXPECT_EQ(iris::scenes::internal::internal::kNumSplitsToEvaluate / 2,
            left_child.Shapes().second);
  auto& right_child = bvh.at(0).RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(iris::scenes::internal::internal::kNumSplitsToEvaluate / 2,
            right_child.Shapes().first);
  EXPECT_EQ(iris::scenes::internal::internal::kNumSplitsToEvaluate / 2,
            right_child.Shapes().second);

  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    EXPECT_EQ(i, geometry_sort_order.at(i));
  }
}

TEST(BuildBVH, NoGeometry) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  auto result =
      iris::scenes::internal::BuildBVH(WrapGeometry(geometry), geometry.size());
  EXPECT_TRUE(result.bvh.empty());
  EXPECT_TRUE(result.geometry_sort_order.empty());
}

TEST(BuildBVH, FullTwoGeometry) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < 2; i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, (2 - i) * 2, 0.0),
                              iris::Point(1.0, (2 - i) * 2 + 1, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(nullptr))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto result =
      iris::scenes::internal::BuildBVH(WrapGeometry(geometry), geometry.size());

  ASSERT_FALSE(result.bvh.empty());
  EXPECT_TRUE(result.bvh[0].HasChildren());
  EXPECT_EQ(iris::Vector::Y_AXIS, result.bvh[0].Axis());
  auto& left_child = result.bvh[0].LeftChild();
  EXPECT_FALSE(left_child.HasChildren());
  EXPECT_EQ(0u, left_child.Shapes().first);
  EXPECT_EQ(1u, left_child.Shapes().second);
  auto& right_child = result.bvh[0].RightChild();
  EXPECT_FALSE(right_child.HasChildren());
  EXPECT_EQ(1u, right_child.Shapes().first);
  EXPECT_EQ(1u, right_child.Shapes().second);

  EXPECT_EQ(1u, result.geometry_sort_order.at(0));
  EXPECT_EQ(0u, result.geometry_sort_order.at(1));
}