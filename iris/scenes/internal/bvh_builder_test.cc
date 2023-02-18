#include "iris/scenes/internal/bvh_builder.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/reference_counted.h"

TEST(ComputeBoundsTest, NoMatrix) {
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

TEST(ComputeBoundsTest, WithMatrix) {
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