#include "iris/bounding_box.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace {

TEST(BoundingBoxBuilderTest, Empty) {
  BoundingBox::Builder builder;
  EXPECT_TRUE(builder.Build().Empty());
}

TEST(BoundingBoxBuilderTest, OnePoint) {
  Point point(0.0, 1.0, 2.0);
  BoundingBox::Builder builder;
  builder.Add(point);
  EXPECT_TRUE(builder.Build().Empty());
}

TEST(BoundingBoxBuilderTest, TwoPoints) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(1.0, 2.0, 3.0);
  BoundingBox::Builder builder;
  builder.Add(point0);
  builder.Add(point1);
  BoundingBox bounds = builder.Build();
  EXPECT_EQ(point0, bounds.lower);
  EXPECT_EQ(point1, bounds.upper);
}

TEST(BoundingBoxBuilderTest, AddEmptyBounds) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(1.0, 2.0, 3.0);
  BoundingBox::Builder builder;
  builder.Add(BoundingBox(point0, point0));
  builder.Add(BoundingBox(point1, point1));
  EXPECT_TRUE(builder.Build().Empty());
}

TEST(BoundingBoxBuilderTest, AddBounds) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(1.0, 2.0, 3.0);
  BoundingBox::Builder builder;
  builder.Add(BoundingBox(point0, point1));
  BoundingBox bounds = builder.Build();
  EXPECT_EQ(point0, bounds.lower);
  EXPECT_EQ(point1, bounds.upper);
}

TEST(BoundingBoxBuilderTest, Reset) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(1.0, 2.0, 3.0);
  BoundingBox::Builder builder;
  builder.Add(point0);
  builder.Add(point1);
  builder.Reset();
  EXPECT_TRUE(builder.Build().Empty());
}

TEST(BoundingBoxTest, CreateOne) {
  Point point(0.0, 1.0, 2.0);
  BoundingBox bounding_box(point);
  EXPECT_EQ(point, bounding_box.upper);
  EXPECT_EQ(point, bounding_box.lower);
}

TEST(BoundingBoxTest, CreateTwo) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(3.0, 4.0, 5.0);
  BoundingBox bounding_box0(point0, point1);
  EXPECT_EQ(point0, bounding_box0.lower);
  EXPECT_EQ(point1, bounding_box0.upper);

  BoundingBox bounding_box1(point1, point0);
  EXPECT_EQ(point0, bounding_box1.lower);
  EXPECT_EQ(point1, bounding_box1.upper);
}

TEST(BoundingBoxTest, CreateThree) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(2.0, 0.0, 1.0);
  Point point2(1.0, 2.0, 0.0);
  BoundingBox bounding_box(point0, point1, point2);

  EXPECT_EQ(Point(0.0, 0.0, 0.0), bounding_box.lower);
  EXPECT_EQ(Point(2.0, 2.0, 2.0), bounding_box.upper);
}

TEST(BoundingBoxTest, CreateOneArray) {
  Point point(0.0, 1.0, 2.0);
  BoundingBox bounding_box({{point}});
  EXPECT_EQ(point, bounding_box.upper);
  EXPECT_EQ(point, bounding_box.lower);
}

TEST(BoundingBoxTest, CreateTwoArray) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(3.0, 4.0, 5.0);
  BoundingBox bounding_box0({{point0, point1}});
  EXPECT_EQ(point0, bounding_box0.lower);
  EXPECT_EQ(point1, bounding_box0.upper);

  BoundingBox bounding_box1({{point1, point0}});
  EXPECT_EQ(point0, bounding_box1.lower);
  EXPECT_EQ(point1, bounding_box1.upper);
}

TEST(BoundingBoxTest, CreateThreeArray) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(2.0, 0.0, 1.0);
  Point point2(1.0, 2.0, 0.0);
  BoundingBox bounding_box({{point0, point1, point2}});

  EXPECT_EQ(Point(0.0, 0.0, 0.0), bounding_box.lower);
  EXPECT_EQ(Point(2.0, 2.0, 2.0), bounding_box.upper);
}

TEST(BoundingBoxTest, Empty) {
  EXPECT_TRUE(BoundingBox(Point(0.0, 1.0, 2.0), Point(0.0, 1.0, 2.0)).Empty());
  EXPECT_TRUE(BoundingBox(Point(0.0, 1.0, 2.0), Point(0.0, 1.0, 3.0)).Empty());
  EXPECT_FALSE(BoundingBox(Point(0.0, 1.0, 2.0), Point(0.0, 2.0, 3.0)).Empty());
  EXPECT_FALSE(BoundingBox(Point(0.0, 1.0, 2.0), Point(1.0, 2.0, 3.0)).Empty());
}

TEST(BoundingBoxTest, Center) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(2.0, 3.0, 4.0);
  BoundingBox bounding_box(point0, point1);
  EXPECT_EQ(Point(1.0, 2.0, 3.0), bounding_box.Center());
}

TEST(BoundingBoxTest, SurfaceArea) {
  Point point0(0.0, 1.0, 2.0);
  Point point1(3.0, 4.0, 5.0);
  BoundingBox bounding_box(point0, point1);
  EXPECT_EQ(54.0, bounding_box.SurfaceArea());
}

TEST(BoundingBoxTest, IntersectInFront) {
  Point point0(-1.0, -1.0, -1.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box(point0, point1);

  Ray ray(Point(0.0, 0.0, -2.0), Vector(0.0, 0.0, 1.0));
  EXPECT_TRUE(bounding_box.Intersects(ray, 0.0, 10000.0));
}

TEST(BoundingBoxTest, IntersectInFrontReversedDistances) {
  Point point0(-1.0, -1.0, -1.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box(point0, point1);

  Ray ray(Point(0.0, 0.0, -2.0), Vector(0.0, 0.0, 1.0));
  EXPECT_FALSE(bounding_box.Intersects(ray, 10000.0, 0.0));
}

TEST(BoundingBoxTest, TooFar) {
  Point point0(-1.0, -1.0, -1.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box(point0, point1);

  Ray ray(Point(0.0, 0.0, -2000.0), Vector(0.0, 0.0, 1.0));
  EXPECT_FALSE(bounding_box.Intersects(ray, 0.0, 10.0));
}

TEST(BoundingBoxTest, IntersectEmpty) {
  Point point0(-1.0, -1.0, -1.0);
  BoundingBox bounding_box(point0);
  Ray ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  EXPECT_FALSE(bounding_box.Intersects(ray, 0.0, 10000.0));
}

TEST(BoundingBoxTest, IntersectInside) {
  Point point0(-1.0, -1.0, -1.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box(point0, point1);

  Ray ray(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
  EXPECT_TRUE(bounding_box.Intersects(ray, 0.0, 10000.0));
}

TEST(BoundingBoxTest, IntersectThin) {
  Point point0(-1.0, -1.0, 0.0);
  Point point1(1.0, 1.0, 0.0);
  BoundingBox bounding_box(point0, point1);

  Ray ray(Point(0.0, 0.0, -1.0), Vector(0.0, 0.0, 1.0));
  EXPECT_TRUE(bounding_box.Intersects(ray, 0.0, 10000.0));
}

TEST(BoundingBoxTest, IntersectBehind) {
  Point point0(-1.0, -1.0, -1.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box(point0, point1);

  Ray ray(Point(0.0, 0.0, 2.0), Vector(0.0, 0.0, 1.0));
  EXPECT_FALSE(bounding_box.Intersects(ray, 0.0, 10000.0));
}

TEST(BoundingBoxTest, IntersectBehindOK) {
  Point point0(-1.0, -1.0, -1.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box(point0, point1);

  Ray ray(Point(0.0, 0.0, 2.0), Vector(0.0, 0.0, 1.0));
  EXPECT_TRUE(bounding_box.Intersects(ray, -10000.0, 10000.0));
}

TEST(BoundingBoxTest, NoIntersection) {
  Point point0(0.0, 0.0, 0.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box0(point0, point1);

  Point point2(2.0, 2.0, 2.0);
  Point point3(3.0, 3.0, 3.0);
  BoundingBox bounding_box1(point2, point3);

  EXPECT_TRUE(Intersect(bounding_box0, bounding_box1).Empty());
}

TEST(BoundingBoxTest, Intersect) {
  Point point0(0.0, 0.0, 0.0);
  Point point2(2.0, 2.0, 2.0);
  BoundingBox bounding_box0(point0, point2);

  Point point1(1.0, 1.0, 1.0);
  Point point3(3.0, 3.0, 3.0);
  BoundingBox bounding_box1(point1, point3);

  BoundingBox intersection = Intersect(bounding_box0, bounding_box1);
  EXPECT_EQ(point1, intersection.lower);
  EXPECT_EQ(point2, intersection.upper);
}

TEST(BoundingBoxTest, JoinLeftEmpty) {
  Point point0(0.0, 0.0, 0.0);
  BoundingBox bounding_box0(point0, point0);

  Point point2(2.0, 2.0, 2.0);
  Point point3(3.0, 3.0, 3.0);
  BoundingBox bounding_box1(point2, point3);

  BoundingBox bounding_box2 = Join(bounding_box0, bounding_box1);
  EXPECT_EQ(point2, bounding_box2.lower);
  EXPECT_EQ(point3, bounding_box2.upper);
}

TEST(BoundingBoxTest, JoinRightEmpty) {
  Point point0(0.0, 0.0, 0.0);
  Point point2(2.0, 2.0, 2.0);
  BoundingBox bounding_box0(point0, point2);

  Point point3(3.0, 3.0, 3.0);
  BoundingBox bounding_box1(point3, point3);

  BoundingBox bounding_box2 = Join(bounding_box0, bounding_box1);
  EXPECT_EQ(point0, bounding_box2.lower);
  EXPECT_EQ(point2, bounding_box2.upper);
}

TEST(BoundingBoxTest, Join) {
  Point point0(0.0, 0.0, 0.0);
  Point point1(1.0, 1.0, 1.0);
  BoundingBox bounding_box0(point0, point1);

  Point point2(2.0, 2.0, 2.0);
  Point point3(3.0, 3.0, 3.0);
  BoundingBox bounding_box1(point2, point3);

  BoundingBox bounding_box2 = Join(bounding_box0, bounding_box1);
  EXPECT_EQ(point0, bounding_box2.lower);
  EXPECT_EQ(point3, bounding_box2.upper);
}

}  // namespace
}  // namespace iris
