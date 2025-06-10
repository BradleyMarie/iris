#include "iris/position_error.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace {

TEST(PositionErrorTest, Create) {
  auto point = PositionError(1.0, 2.0, 3.0);
  EXPECT_EQ(1.0, point.x);
  EXPECT_EQ(2.0, point.y);
  EXPECT_EQ(3.0, point.z);
}

TEST(PositionErrorTest, AddPositionError) {
  auto addend0 = PositionError(3.0, 4.0, 5.0);
  auto addend1 = PositionError(2.0, 2.0, 3.0);
  EXPECT_EQ(PositionError(5.0, 6.0, 8.0), addend0 + addend1);
}

TEST(PositionErrorTest, ScalePositionError) {
  auto value = PositionError(3.0, 4.0, 5.0);
  EXPECT_EQ(PositionError(6.0, 8.0, 10.0), value * 2.0);
}

TEST(PositionErrorTest, RoundingError) {
  EXPECT_EQ(0.0, RoundingError(0));
  for (uint8_t i = 0; i < std::numeric_limits<uint8_t>::max(); i++) {
    EXPECT_LT(RoundingError(i + 1 - 1), RoundingError(i + 1));
  }
}

}  // namespace
}  // namespace iris
