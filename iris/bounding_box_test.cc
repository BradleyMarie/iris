#include "iris/bounding_box.h"

#include "googletest/include/gtest/gtest.h"

TEST(BoundingBoxTest, CreateOne) {
  iris::Point point(0.0, 1.0, 2.0);
  iris::BoundingBox bounding_box(point);
  EXPECT_EQ(point, bounding_box.upper);
  EXPECT_EQ(point, bounding_box.lower);
}

TEST(BoundingBoxTest, CreateTwo) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(3.0, 4.0, 5.0);
  iris::BoundingBox bounding_box0(point0, point1);
  EXPECT_EQ(point0, bounding_box0.lower);
  EXPECT_EQ(point1, bounding_box0.upper);

  iris::BoundingBox bounding_box1(point1, point0);
  EXPECT_EQ(point0, bounding_box1.lower);
  EXPECT_EQ(point1, bounding_box1.upper);
}

TEST(BoundingBoxTest, CreateThree) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(2.0, 0.0, 1.0);
  iris::Point point2(1.0, 2.0, 0.0);
  iris::BoundingBox bounding_box(point0, point1, point2);

  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), bounding_box.lower);
  EXPECT_EQ(iris::Point(2.0, 2.0, 2.0), bounding_box.upper);
}

TEST(BoundingBoxTest, Envelop) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(3.0, 4.0, 5.0);
  iris::BoundingBox bounding_box0(point0);
  iris::BoundingBox bounding_box_1 = bounding_box0.Envelop(point1);

  EXPECT_EQ(iris::Point(0.0, 1.0, 2.0), bounding_box_1.lower);
  EXPECT_EQ(iris::Point(3.0, 4.0, 5.0), bounding_box_1.upper);
}

TEST(BoundingBoxTest, SurfaceArea) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(3.0, 4.0, 5.0);
  iris::BoundingBox bounding_box(point0, point1);
  EXPECT_EQ(54.0, bounding_box.SurfaceArea());
}

TEST(BoundingBoxTest, ProbablyIntersectsInFront) {
  iris::Point point0(-1.0, -1.0, -1.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box(point0, point1);

  iris::Ray ray(iris::Point(0.0, 0.0, -2.0), iris::Vector(0.0, 0.0, 1.0));
  auto intersection = bounding_box.Intersects(ray);
  ASSERT_TRUE(intersection);
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[0]));
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[1]));
  EXPECT_EQ(1.0, intersection->inverse_direction[2]);
  EXPECT_EQ(1.0, intersection->begin);
  EXPECT_EQ(3.0, intersection->end);
}

TEST(BoundingBoxTest, IntersectsInside) {
  iris::Point point0(-1.0, -1.0, -1.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box(point0, point1);

  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  auto intersection = bounding_box.Intersects(ray);
  ASSERT_TRUE(intersection);
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[0]));
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[1]));
  EXPECT_EQ(1.0, intersection->inverse_direction[2]);
  EXPECT_EQ(0.0, intersection->begin);
  EXPECT_EQ(1.0, intersection->end);
}

TEST(BoundingBoxTest, IntersectsBehind) {
  iris::Point point0(-1.0, -1.0, -1.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box(point0, point1);

  iris::Ray ray(iris::Point(0.0, 0.0, 2.0), iris::Vector(0.0, 0.0, 1.0));
  auto intersection = bounding_box.Intersects(ray);
  ASSERT_FALSE(intersection);
}

TEST(BoundingBoxTest, NoIntersection) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box0(point0, point1);

  iris::Point point2(2.0, 2.0, 2.0);
  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point2, point3);

  EXPECT_FALSE(iris::Intersection(bounding_box0, bounding_box1));
}

TEST(BoundingBoxTest, Intersection) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::Point point2(2.0, 2.0, 2.0);
  iris::BoundingBox bounding_box0(point0, point2);

  iris::Point point1(1.0, 1.0, 1.0);
  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point1, point3);

  auto intersection = iris::Intersection(bounding_box0, bounding_box1);
  ASSERT_TRUE(intersection);
  EXPECT_EQ(point1, intersection->lower);
  EXPECT_EQ(point2, intersection->upper);
}

TEST(BoundingBoxTest, Union) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box0(point0, point1);

  iris::Point point2(2.0, 2.0, 2.0);
  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point2, point3);

  iris::BoundingBox bounding_box2 = iris::Union(bounding_box0, bounding_box1);
  EXPECT_EQ(point0, bounding_box2.lower);
  EXPECT_EQ(point3, bounding_box2.upper);
}