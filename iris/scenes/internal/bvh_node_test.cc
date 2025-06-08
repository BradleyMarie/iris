#include "iris/scenes/internal/bvh_node.h"

#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/vector.h"

namespace iris {
namespace scenes {
namespace internal {
namespace {

TEST(Bounds, LeafNode) {
  BoundingBox expected_bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BVHNode node = BVHNode::MakeLeafNode(expected_bounds, 100u, 3u);
  EXPECT_EQ(expected_bounds.lower, node.Bounds().lower);
  EXPECT_EQ(expected_bounds.upper, node.Bounds().upper);
}

TEST(BVHNodeTest, LeafNode) {
  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BVHNode node = BVHNode::MakeLeafNode(bounds, 100u, 3u);
  ASSERT_FALSE(node.HasChildren());
  EXPECT_EQ(100u, node.Shapes().first);
  EXPECT_EQ(3u, node.Shapes().second);
}

TEST(BVHNodeTest, InteriorNode) {
  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  std::vector<BVHNode> nodes(60,
                             BVHNode::MakeInteriorNode(bounds, Vector::X_AXIS));
  nodes[0].SetRightChildOffset(30);
  ASSERT_TRUE(nodes[0].HasChildren());
  EXPECT_EQ(&nodes[0] + 1, &nodes[0].LeftChild());
  EXPECT_EQ(&nodes[0] + 30, &nodes[0].RightChild());
  EXPECT_EQ(Vector::X_AXIS, nodes[0].Axis());
}

TEST(BVHNodeTest, Intersects) {
  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BVHNode node = BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_TRUE(node.Intersects(Ray(Point(0.5, 0.5, -1.0), Vector(0.0, 0.0, 1.0)),
                              0.0, 2.0));
}

TEST(BVHNodeTest, IntersectsWrongDirection) {
  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BVHNode node = BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_FALSE(node.Intersects(
      Ray(Point(0.5, 0.5, -1.0), Vector(0.0, 0.0, -1.0)), 0.0, 2.0));
}

TEST(BVHNodeTest, IntersectsTooClose) {
  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BVHNode node = BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_FALSE(node.Intersects(
      Ray(Point(0.5, 0.5, -1.0), Vector(0.0, 0.0, 1.0)), 3.0, 4.0));
}

TEST(BVHNodeTest, IntersectsTooFar) {
  BoundingBox bounds(Point(0.0, 0.0, 0.0), Point(1.0, 1.0, 1.0));
  BVHNode node = BVHNode::MakeLeafNode(bounds, 100u, 3u);
  EXPECT_FALSE(node.Intersects(
      Ray(Point(0.5, 0.5, -1.0), Vector(0.0, 0.0, 1.0)), 0.0, 0.5));
}

}  // namespace
}  // namespace internal
}  // namespace scenes
}  // namespace iris
