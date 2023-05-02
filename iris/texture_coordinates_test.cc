#include "iris/texture_coordinates.h"

#include "googletest/include/gtest/gtest.h"

TEST(Scale, NoDerivatives) {
  iris::TextureCoordinates coordinates{{2.0, 3.0}};
  auto scaled = coordinates.Scale(0.5, 2.0, 1.0, 2.0);
  EXPECT_EQ(2.0, scaled.uv[0]);
  EXPECT_EQ(8.0, scaled.uv[1]);
  EXPECT_FALSE(scaled.derivatives.has_value());
}

TEST(Scale, WithDerivatives) {
  iris::TextureCoordinates coordinates{
      {2.0, 3.0},
      {{2.0, 3.0, 4.0, 5.0, iris::Vector(2.0, 4.0, 8.0),
        iris::Vector(16.0, 32.0, 64.0)}}};
  auto scaled = coordinates.Scale(2.0, 4.0, 1.0, 2.0);
  EXPECT_EQ(5.0, scaled.uv[0]);
  EXPECT_EQ(14.0, scaled.uv[1]);
  ASSERT_TRUE(scaled.derivatives.has_value());
  EXPECT_EQ(4.0, (*scaled.derivatives).du_dx);
  EXPECT_EQ(6.0, (*scaled.derivatives).du_dy);
  EXPECT_EQ(16.0, (*scaled.derivatives).dv_dx);
  EXPECT_EQ(20.0, (*scaled.derivatives).dv_dy);
  EXPECT_EQ(1.0, (*scaled.derivatives).dp_du.x);
  EXPECT_EQ(2.0, (*scaled.derivatives).dp_du.y);
  EXPECT_EQ(4.0, (*scaled.derivatives).dp_du.z);
  EXPECT_EQ(4.0, (*scaled.derivatives).dp_dv.x);
  EXPECT_EQ(8.0, (*scaled.derivatives).dp_dv.y);
  EXPECT_EQ(16.0, (*scaled.derivatives).dp_dv.z);
}