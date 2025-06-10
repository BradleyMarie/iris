#include "iris/ray_differential.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/ray.h"

namespace iris {
namespace {

TEST(RayTest, Create) {
  Ray base(Point(1.0, 2.0, 3.0), Vector(4.0, 5.0, 6.0));
  RayDifferential ray = RayDifferential(base);
  EXPECT_EQ(base, ray);
  EXPECT_FALSE(ray.differentials);
}

TEST(RayTest, CreateWithDifferentials) {
  Ray base(Point(1.0, 2.0, 3.0), Vector(4.0, 5.0, 6.0));
  Ray dx(Point(1.0, 2.0, 4.0), Vector(4.0, 5.0, 7.0));
  Ray dy(Point(1.0, 2.0, 5.0), Vector(4.0, 5.0, 8.0));
  RayDifferential ray = RayDifferential(base, dx, dy);
  EXPECT_EQ(base, ray);
  ASSERT_TRUE(ray.differentials);
  EXPECT_EQ(dx, ray.differentials->dx);
  EXPECT_EQ(dy, ray.differentials->dy);
}

TEST(RayTest, Normalize) {
  Ray base(Point(1.0, 2.0, 3.0), Vector(0.0, 0.0, 2.0));
  RayDifferential ray = Normalize(RayDifferential(base));
  EXPECT_EQ(base.origin, ray.origin);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), ray.direction);
  EXPECT_FALSE(ray.differentials);
}

TEST(RayTest, NormalizeWithDifferentials) {
  Ray base(Point(1.0, 2.0, 3.0), Vector(0.0, 0.0, 2.0));
  Ray dx(Point(1.0, 2.0, 4.0), Vector(2.0, 0.0, 0.0));
  Ray dy(Point(1.0, 2.0, 5.0), Vector(0.0, 2.0, 0.0));
  RayDifferential ray = Normalize(RayDifferential(base, dx, dy));
  EXPECT_EQ(base.origin, ray.origin);
  EXPECT_EQ(Vector(0.0, 0.0, 1.0), ray.direction);
  ASSERT_TRUE(ray.differentials);
  EXPECT_EQ(dx.origin, ray.differentials->dx.origin);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), ray.differentials->dx.direction);
  EXPECT_EQ(dy.origin, ray.differentials->dy.origin);
  EXPECT_EQ(Vector(0.0, 1.0, 0.0), ray.differentials->dy.direction);
}

}  // namespace
}  // namespace iris
