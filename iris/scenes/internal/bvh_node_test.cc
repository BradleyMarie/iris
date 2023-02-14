#include "iris/scenes/internal/bvh_node.h"

#include "googletest/include/gtest/gtest.h"

TEST(BVHNodeTest, LeafNode) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto node = iris::scenes::internal::BVHNode::MakeLeafNode(bounds, 100u, 3u);
  ASSERT_FALSE(node.HasChildren());
  EXPECT_EQ(100u, node.Shapes().first);
  EXPECT_EQ(3u, node.Shapes().second);
}

TEST(BVHNodeTest, InteriorNode) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto node = iris::scenes::internal::BVHNode::MakeInteriorNode(
      bounds, iris::Vector::X_AXIS);
  node.SetRightChildOffset(30);
  ASSERT_TRUE(node.HasChildren());
  EXPECT_EQ(&node + 1, node.LeftChild());
  EXPECT_EQ(&node + 30, node.RightChild());
  EXPECT_EQ(iris::Vector::X_AXIS, node.Axis());
}

TEST(BVHNodeTest, Intersects) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto node = iris::scenes::internal::BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_TRUE(node.Intersects(
      iris::Ray(iris::Point(0.5, 0.5, -1.0), iris::Vector(0.0, 0.0, 1.0))));
  EXPECT_FALSE(node.Intersects(
      iris::Ray(iris::Point(0.5, 0.5, -1.0), iris::Vector(0.0, 0.0, -1.0))));
}