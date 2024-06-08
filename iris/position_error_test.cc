#include "iris/position_error.h"

#include "googletest/include/gtest/gtest.h"

TEST(PositionErrorTest, Create) {
  auto point = iris::PositionError(1.0, 2.0, 3.0);
  EXPECT_EQ(1.0, point.x);
  EXPECT_EQ(2.0, point.y);
  EXPECT_EQ(3.0, point.z);
}

TEST(PositionErrorTest, AddPositionError) {
  auto addend0 = iris::PositionError(3.0, 4.0, 5.0);
  auto addend1 = iris::PositionError(2.0, 2.0, 3.0);
  EXPECT_EQ(iris::PositionError(5.0, 6.0, 8.0), addend0 + addend1);
}

TEST(PositionErrorTest, ScalePositionError) {
  auto value = iris::PositionError(3.0, 4.0, 5.0);
  EXPECT_EQ(iris::PositionError(6.0, 8.0, 10.0), value * 2.0);
}

TEST(PositionErrorTest, RoundingError) {
  EXPECT_EQ(0.0, iris::RoundingError(0));
  for (uint8_t i = 0; i < std::numeric_limits<uint8_t>::max(); i++) {
    EXPECT_LT(iris::RoundingError(i + 1 - 1), iris::RoundingError(i + 1));
  }
}