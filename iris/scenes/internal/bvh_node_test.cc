#include "iris/scenes/internal/bvh_node.h"

#include "googletest/include/gtest/gtest.h"

TEST(Bounds, LeafNode) {
  iris::BoundingBox expected_bounds(iris::Point(0.0, 0.0, 0.0),
                                    iris::Point(1.0, 1.0, 1.0));
  auto node =
      iris::scenes::internal::BVHNode::MakeLeafNode(expected_bounds, 100u, 3u);
  EXPECT_EQ(expected_bounds.lower, node.Bounds().lower);
  EXPECT_EQ(expected_bounds.upper, node.Bounds().upper);
}

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
  std::vector<iris::scenes::internal::BVHNode> nodes(
      60, iris::scenes::internal::BVHNode::MakeInteriorNode(
              bounds, iris::Vector::X_AXIS));
  nodes[0].SetRightChildOffset(30);
  ASSERT_TRUE(nodes[0].HasChildren());
  EXPECT_EQ(&nodes[0] + 1, &nodes[0].LeftChild());
  EXPECT_EQ(&nodes[0] + 30, &nodes[0].RightChild());
  EXPECT_EQ(iris::Vector::X_AXIS, nodes[0].Axis());
}

TEST(BVHNodeTest, Intersects) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto node = iris::scenes::internal::BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_TRUE(node.Intersects(
      iris::Ray(iris::Point(0.5, 0.5, -1.0), iris::Vector(0.0, 0.0, 1.0)), 0.0,
      2.0));
}

TEST(BVHNodeTest, IntersectsWrongDirection) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto node = iris::scenes::internal::BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_FALSE(node.Intersects(
      iris::Ray(iris::Point(0.5, 0.5, -1.0), iris::Vector(0.0, 0.0, -1.0)), 0.0,
      2.0));
}

TEST(BVHNodeTest, IntersectsTooClose) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto node = iris::scenes::internal::BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_FALSE(node.Intersects(
      iris::Ray(iris::Point(0.5, 0.5, -1.0), iris::Vector(0.0, 0.0, 1.0)), 3.0,
      4.0));
}

TEST(BVHNodeTest, IntersectsTooFar) {
  iris::BoundingBox bounds(iris::Point(0.0, 0.0, 0.0),
                           iris::Point(1.0, 1.0, 1.0));
  auto node = iris::scenes::internal::BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_FALSE(node.Intersects(
      iris::Ray(iris::Point(0.5, 0.5, -1.0), iris::Vector(0.0, 0.0, 1.0)), 0.0,
      0.5));
}