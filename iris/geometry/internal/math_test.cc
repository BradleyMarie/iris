#include "iris/geometry/internal/math.h"

#include "googletest/include/gtest/gtest.h"

TEST(Gamma, Gamma) {
  EXPECT_EQ(0.0, iris::geometry::internal::Gamma(0));
  EXPECT_NE(0.0, iris::geometry::internal::Gamma(1));
}