#include "iris/point.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/vector.h"

namespace iris {
namespace {

TEST(PointTest, Create) {
  Point point(1.0, 2.0, 3.0);
  EXPECT_EQ(1.0, point.x);
  EXPECT_EQ(2.0, point.y);
  EXPECT_EQ(3.0, point.z);
}

TEST(PointTest, SubtractPoint) {
  Point minuend(3.0, 4.0, 5.0);
  Point subtrahend(2.0, 1.0, 3.0);
  EXPECT_EQ(Vector(1.0, 3.0, 2.0), minuend - subtrahend);
}

TEST(PointTest, SubtractVector) {
  Point minuend(3.0, 4.0, 5.0);
  Vector subtrahend(2.0, 1.0, 3.0);
  EXPECT_EQ(Point(1.0, 3.0, 2.0), minuend - subtrahend);
}

TEST(PointTest, Add) {
  Point addend0(3.0, 4.0, 5.0);
  Vector addend1(2.0, 2.0, 3.0);
  EXPECT_EQ(Point(5.0, 6.0, 8.0), addend0 + addend1);
}

}  // namespace
}  // namespace iris
