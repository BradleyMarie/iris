#include "iris/ray.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {
namespace {

TEST(RayTest, Create) {
  Point origin(1.0, 2.0, 3.0);
  Vector direction(4.0, 5.0, 6.0);
  Ray ray(origin, direction);
  EXPECT_EQ(origin, ray.origin);
  EXPECT_EQ(direction, ray.direction);
}

TEST(RayTest, Endpoint) {
  Point origin(1.0, 2.0, 3.0);
  Vector direction(4.0, 5.0, 6.0);
  Ray ray(origin, direction);
  EXPECT_EQ(Point(9.0, 12.0, 15.0), ray.Endpoint(2.0));
}

TEST(RayTest, Normalize) {
  Point origin(1.0, 2.0, 3.0);
  EXPECT_EQ(Ray(origin, Vector(1.0, 0.0, 0.0)),
            Normalize(Ray(origin, Vector(2.0, 0.0, 0.0))));
  EXPECT_EQ(Ray(origin, Vector(0.0, 1.0, 0.0)),
            Normalize(Ray(origin, Vector(0.0, 1.0, 0.0))));
  EXPECT_EQ(Ray(origin, Vector(0.0, 0.0, 1.0)),
            Normalize(Ray(origin, Vector(0.0, 0.0, 1.0))));
}

}  // namespace
}  // namespace iris
