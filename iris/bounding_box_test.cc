#include "iris/bounding_box.h"

#include "googletest/include/gtest/gtest.h"

TEST(BoundingBoxBuilderTest, Empty) {
  iris::BoundingBox::Builder builder;
  EXPECT_TRUE(builder.Build().Empty());
}

TEST(BoundingBoxBuilderTest, OnePoint) {
  iris::Point point(0.0, 1.0, 2.0);
  iris::BoundingBox::Builder builder;
  builder.Add(point);
  EXPECT_TRUE(builder.Build().Empty());
}

TEST(BoundingBoxBuilderTest, TwoPoints) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(1.0, 2.0, 3.0);
  iris::BoundingBox::Builder builder;
  builder.Add(point0);
  builder.Add(point1);
  auto bounds = builder.Build();
  EXPECT_EQ(point0, bounds.lower);
  EXPECT_EQ(point1, bounds.upper);
}

TEST(BoundingBoxBuilderTest, AddEmptyBounds) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(1.0, 2.0, 3.0);
  iris::BoundingBox::Builder builder;
  builder.Add(iris::BoundingBox(point0, point0));
  builder.Add(iris::BoundingBox(point1, point1));
  EXPECT_TRUE(builder.Build().Empty());
}

TEST(BoundingBoxBuilderTest, AddBounds) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(1.0, 2.0, 3.0);
  iris::BoundingBox::Builder builder;
  builder.Add(iris::BoundingBox(point0, point1));
  auto bounds = builder.Build();
  EXPECT_EQ(point0, bounds.lower);
  EXPECT_EQ(point1, bounds.upper);
}

TEST(BoundingBoxBuilderTest, Reset) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(1.0, 2.0, 3.0);
  iris::BoundingBox::Builder builder;
  builder.Add(point0);
  builder.Add(point1);
  builder.Reset();
  EXPECT_TRUE(builder.Build().Empty());
}

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

TEST(BoundingBoxTest, CreateOneArray) {
  iris::Point point(0.0, 1.0, 2.0);
  iris::BoundingBox bounding_box({{point}});
  EXPECT_EQ(point, bounding_box.upper);
  EXPECT_EQ(point, bounding_box.lower);
}

TEST(BoundingBoxTest, CreateTwoArray) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(3.0, 4.0, 5.0);
  iris::BoundingBox bounding_box0({{point0, point1}});
  EXPECT_EQ(point0, bounding_box0.lower);
  EXPECT_EQ(point1, bounding_box0.upper);

  iris::BoundingBox bounding_box1({{point1, point0}});
  EXPECT_EQ(point0, bounding_box1.lower);
  EXPECT_EQ(point1, bounding_box1.upper);
}

TEST(BoundingBoxTest, CreateThreeArray) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(2.0, 0.0, 1.0);
  iris::Point point2(1.0, 2.0, 0.0);
  iris::BoundingBox bounding_box({{point0, point1, point2}});

  EXPECT_EQ(iris::Point(0.0, 0.0, 0.0), bounding_box.lower);
  EXPECT_EQ(iris::Point(2.0, 2.0, 2.0), bounding_box.upper);
}

TEST(BoundingBoxTest, Empty) {
  EXPECT_TRUE(
      iris::BoundingBox(iris::Point(0.0, 1.0, 2.0), iris::Point(0.0, 1.0, 2.0))
          .Empty());
  EXPECT_TRUE(
      iris::BoundingBox(iris::Point(0.0, 1.0, 2.0), iris::Point(0.0, 1.0, 3.0))
          .Empty());
  EXPECT_FALSE(
      iris::BoundingBox(iris::Point(0.0, 1.0, 2.0), iris::Point(0.0, 2.0, 3.0))
          .Empty());
  EXPECT_FALSE(
      iris::BoundingBox(iris::Point(0.0, 1.0, 2.0), iris::Point(1.0, 2.0, 3.0))
          .Empty());
}

TEST(BoundingBoxTest, SurfaceArea) {
  iris::Point point0(0.0, 1.0, 2.0);
  iris::Point point1(3.0, 4.0, 5.0);
  iris::BoundingBox bounding_box(point0, point1);
  EXPECT_EQ(54.0, bounding_box.SurfaceArea());
}

TEST(BoundingBoxTest, ProbablyIntersectInFront) {
  iris::Point point0(-1.0, -1.0, -1.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box(point0, point1);

  iris::Ray ray(iris::Point(0.0, 0.0, -2.0), iris::Vector(0.0, 0.0, 1.0));
  auto intersection = bounding_box.Intersect(ray);
  ASSERT_TRUE(intersection);
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[0]));
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[1]));
  EXPECT_EQ(1.0, intersection->inverse_direction[2]);
  EXPECT_EQ(1.0, intersection->begin);
  EXPECT_EQ(3.0, intersection->end);
}

TEST(BoundingBoxTest, IntersectEmpty) {
  iris::Point point0(-1.0, -1.0, -1.0);
  iris::BoundingBox bounding_box(point0);
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  EXPECT_FALSE(bounding_box.Intersect(ray).has_value());
}

TEST(BoundingBoxTest, IntersectInside) {
  iris::Point point0(-1.0, -1.0, -1.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box(point0, point1);

  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.0, 0.0, 1.0));
  auto intersection = bounding_box.Intersect(ray);
  ASSERT_TRUE(intersection);
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[0]));
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[1]));
  EXPECT_EQ(1.0, intersection->inverse_direction[2]);
  EXPECT_EQ(0.0, intersection->begin);
  EXPECT_EQ(1.0, intersection->end);
}

TEST(BoundingBoxTest, IntersectThin) {
  iris::Point point0(-1.0, -1.0, 0.0);
  iris::Point point1(1.0, 1.0, 0.0);
  iris::BoundingBox bounding_box(point0, point1);

  iris::Ray ray(iris::Point(0.0, 0.0, -1.0), iris::Vector(0.0, 0.0, 1.0));
  auto intersection = bounding_box.Intersect(ray);
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[0]));
  EXPECT_TRUE(std::isinf(intersection->inverse_direction[1]));
  EXPECT_EQ(1.0, intersection->inverse_direction[2]);
  EXPECT_EQ(1.0, intersection->begin);
  EXPECT_EQ(1.0, intersection->end);
}

TEST(BoundingBoxTest, IntersectBehind) {
  iris::Point point0(-1.0, -1.0, -1.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box(point0, point1);

  iris::Ray ray(iris::Point(0.0, 0.0, 2.0), iris::Vector(0.0, 0.0, 1.0));
  auto intersection = bounding_box.Intersect(ray);
  ASSERT_FALSE(intersection);
}

TEST(BoundingBoxTest, NoIntersection) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box0(point0, point1);

  iris::Point point2(2.0, 2.0, 2.0);
  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point2, point3);

  EXPECT_TRUE(iris::Intersect(bounding_box0, bounding_box1).Empty());
}

TEST(BoundingBoxTest, Intersect) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::Point point2(2.0, 2.0, 2.0);
  iris::BoundingBox bounding_box0(point0, point2);

  iris::Point point1(1.0, 1.0, 1.0);
  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point1, point3);

  auto intersection = iris::Intersect(bounding_box0, bounding_box1);
  EXPECT_EQ(point1, intersection.lower);
  EXPECT_EQ(point2, intersection.upper);
}

TEST(BoundingBoxTest, JoinLeftEmpty) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::BoundingBox bounding_box0(point0, point0);

  iris::Point point2(2.0, 2.0, 2.0);
  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point2, point3);

  iris::BoundingBox bounding_box2 = iris::Join(bounding_box0, bounding_box1);
  EXPECT_EQ(point2, bounding_box2.lower);
  EXPECT_EQ(point3, bounding_box2.upper);
}

TEST(BoundingBoxTest, JoinRightEmpty) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::Point point2(2.0, 2.0, 2.0);
  iris::BoundingBox bounding_box0(point0, point2);

  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point3, point3);

  iris::BoundingBox bounding_box2 = iris::Join(bounding_box0, bounding_box1);
  EXPECT_EQ(point0, bounding_box2.lower);
  EXPECT_EQ(point2, bounding_box2.upper);
}

TEST(BoundingBoxTest, Join) {
  iris::Point point0(0.0, 0.0, 0.0);
  iris::Point point1(1.0, 1.0, 1.0);
  iris::BoundingBox bounding_box0(point0, point1);

  iris::Point point2(2.0, 2.0, 2.0);
  iris::Point point3(3.0, 3.0, 3.0);
  iris::BoundingBox bounding_box1(point2, point3);

  iris::BoundingBox bounding_box2 = iris::Join(bounding_box0, bounding_box1);
  EXPECT_EQ(point0, bounding_box2.lower);
  EXPECT_EQ(point3, bounding_box2.upper);
}