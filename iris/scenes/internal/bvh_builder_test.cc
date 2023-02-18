#include "iris/scenes/internal/bvh_builder.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/reference_counted.h"

TEST(ComputeBounds, NoMatrix) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(bounds));
  auto actual_bounds =
      iris::scenes::internal::internal::ComputeBounds({geometry, nullptr});
  EXPECT_EQ(bounds.lower, actual_bounds.lower);
  EXPECT_EQ(bounds.upper, actual_bounds.upper);
}

TEST(ComputeBounds, WithMatrix) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto transformation = iris::Matrix::Translation(2.0, 2.0, 2.0).value();
  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(transformation))
      .WillOnce(testing::Return(bounds));
  auto actual_bounds = iris::scenes::internal::internal::ComputeBounds(
      {geometry, &transformation});
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
  EXPECT_CALL(*geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(bounds0));
  EXPECT_CALL(*geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(bounds1));
  EXPECT_CALL(*geometry2, ComputeBounds(iris::Matrix::Identity())).Times(0);
  auto actual_bounds = iris::scenes::internal::internal::ComputeBounds(
      {{{geometry0, nullptr}, {geometry1, nullptr}, {geometry2, nullptr}}},
      {{1, 0}});
  EXPECT_EQ(bounds0.lower, actual_bounds.lower);
  EXPECT_EQ(bounds1.upper, actual_bounds.upper);
}

TEST(ComputeCentroid, FromBounds) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto centroid = iris::scenes::internal::internal::ComputeCentroid(bounds);
  EXPECT_EQ(iris::Point(0.5, 0.5, 0.5), centroid);
}

TEST(ComputeCentroid, FromGeometry) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(bounds));
  auto centroid =
      iris::scenes::internal::internal::ComputeCentroid({geometry, nullptr});
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
  EXPECT_CALL(*geometry0, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(bounds0));
  EXPECT_CALL(*geometry1, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(bounds1));
  EXPECT_CALL(*geometry2, ComputeBounds(iris::Matrix::Identity())).Times(0);
  auto actual_bounds = iris::scenes::internal::internal::ComputeCentroidBounds(
      {{{geometry0, nullptr}, {geometry1, nullptr}, {geometry2, nullptr}}},
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
    EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity()))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity())).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto splits = iris::scenes::internal::internal::ComputeSplits(
      geometry, indices,
      iris::scenes::internal::internal::ComputeCentroidBounds(geometry,
                                                              indices),
      iris::Vector::Y_AXIS);
  ASSERT_TRUE(splits.has_value());

  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    EXPECT_EQ(iris::Point(0.0, i, 0.0), splits->at(i).bounds.Build().lower);
    EXPECT_EQ(iris::Point(1.0, i + 1, 1.0), splits->at(i).bounds.Build().upper);
    EXPECT_EQ(1u, splits->at(i).num_shapes);
  }
}

TEST(ComputeSplits, NoResult) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumSplitsToEvaluate;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                              iris::Point(0.0, 1.0, 0.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity()))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity())).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto splits = iris::scenes::internal::internal::ComputeSplits(
      geometry, indices,
      iris::scenes::internal::internal::ComputeCentroidBounds(geometry,
                                                              indices),
      iris::Vector::Y_AXIS);
  ASSERT_FALSE(splits.has_value());
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
    EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity()))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity())).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      geometry, indices,
      iris::scenes::internal::internal::ComputeBounds(geometry, indices),
      iris::Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_EQ(6.0, *best_split);
}

TEST(FindBestSplitOnAxis, NoSplit) {
  std::vector<std::pair<const iris::ReferenceCounted<iris::Geometry>,
                        const iris::Matrix*>>
      geometry;
  std::vector<size_t> indices;
  for (size_t i = 0; i < iris::scenes::internal::internal::kNumShapesPerNode;
       i++) {
    auto mock_geometry =
        iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
    iris::BoundingBox bounds0(iris::Point(0.0, 0.0, 0.0),
                              iris::Point(1.0, 1.0, 1.0));
    EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity()))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity())).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      geometry, indices,
      iris::scenes::internal::internal::ComputeBounds(geometry, indices),
      iris::Vector::Y_AXIS);
  ASSERT_FALSE(best_split.has_value());
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
    EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity()))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity())).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      geometry, indices,
      iris::scenes::internal::internal::ComputeBounds(geometry, indices),
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
    EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity()))
        .WillRepeatedly(testing::Return(bounds0));
    geometry.emplace_back(mock_geometry, nullptr);
    indices.push_back(i);
  }

  auto mock_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*mock_geometry, ComputeBounds(iris::Matrix::Identity())).Times(0);
  geometry.emplace_back(mock_geometry, nullptr);

  auto best_split = iris::scenes::internal::internal::FindBestSplitOnAxis(
      geometry, indices,
      iris::scenes::internal::internal::ComputeBounds(geometry, indices),
      iris::Vector::Y_AXIS);
  ASSERT_TRUE(best_split.has_value());
  EXPECT_NEAR(1.0, *best_split, 0.0000001);
}