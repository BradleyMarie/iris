#include "iris/ray_differential.h"

#include "googletest/include/gtest/gtest.h"

TEST(RayTest, Create) {
  iris::Ray base(iris::Point(1.0, 2.0, 3.0), iris::Vector(4.0, 5.0, 6.0));
  auto ray = iris::RayDifferential(base);
  EXPECT_EQ(base, ray);
  EXPECT_FALSE(ray.differentials);
}

TEST(RayTest, CreateWithDifferentials) {
  iris::Ray base(iris::Point(1.0, 2.0, 3.0), iris::Vector(4.0, 5.0, 6.0));
  iris::Ray dx(iris::Point(1.0, 2.0, 4.0), iris::Vector(4.0, 5.0, 7.0));
  iris::Ray dy(iris::Point(1.0, 2.0, 5.0), iris::Vector(4.0, 5.0, 8.0));
  auto ray = iris::RayDifferential(base, dx, dy);
  EXPECT_EQ(base, ray);
  ASSERT_TRUE(ray.differentials);
  EXPECT_EQ(dx, ray.differentials->dx);
  EXPECT_EQ(dy, ray.differentials->dy);
}

TEST(RayTest, Normalize) {
  iris::Ray base(iris::Point(1.0, 2.0, 3.0), iris::Vector(0.0, 0.0, 2.0));
  auto ray = Normalize(iris::RayDifferential(base));
  EXPECT_EQ(base.origin, ray.origin);
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), ray.direction);
  EXPECT_FALSE(ray.differentials);
}

TEST(RayTest, NormalizeWithDifferentials) {
  iris::Ray base(iris::Point(1.0, 2.0, 3.0), iris::Vector(0.0, 0.0, 2.0));
  iris::Ray dx(iris::Point(1.0, 2.0, 4.0), iris::Vector(2.0, 0.0, 0.0));
  iris::Ray dy(iris::Point(1.0, 2.0, 5.0), iris::Vector(0.0, 2.0, 0.0));
  auto ray = Normalize(iris::RayDifferential(base, dx, dy));
  EXPECT_EQ(base.origin, ray.origin);
  EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), ray.direction);
  ASSERT_TRUE(ray.differentials);
  EXPECT_EQ(dx.origin, ray.differentials->dx.origin);
  EXPECT_EQ(iris::Vector(1.0, 0.0, 0.0), ray.differentials->dx.direction);
  EXPECT_EQ(dy.origin, ray.differentials->dy.origin);
  EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), ray.differentials->dy.direction);
}
