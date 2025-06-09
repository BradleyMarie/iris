#include "iris/texture_coordinates.h"

#include "googletest/include/gtest/gtest.h"

TEST(Scale, NoDerivatives) {
  iris::TextureCoordinates coordinates{{2.0, 3.0}};
  auto scaled = coordinates.Scale(0.5, 2.0, 1.0, 2.0);
  EXPECT_EQ(2.0, scaled.uv[0]);
  EXPECT_EQ(8.0, scaled.uv[1]);
  EXPECT_FALSE(scaled.differentials.has_value());
}

TEST(Scale, WithDerivatives) {
  iris::TextureCoordinates coordinates{{2.0, 3.0},
                                       {{
                                           2.0,
                                           3.0,
                                           4.0,
                                           5.0,
                                       }}};
  auto scaled = coordinates.Scale(2.0, 4.0, 1.0, 2.0);
  EXPECT_EQ(5.0, scaled.uv[0]);
  EXPECT_EQ(14.0, scaled.uv[1]);
  ASSERT_TRUE(scaled.differentials.has_value());
  EXPECT_EQ(4.0, (*scaled.differentials).du_dx);
  EXPECT_EQ(6.0, (*scaled.differentials).du_dy);
  EXPECT_EQ(16.0, (*scaled.differentials).dv_dx);
  EXPECT_EQ(20.0, (*scaled.differentials).dv_dy);
}
