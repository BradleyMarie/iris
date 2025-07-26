#include "iris/texture_coordinates.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {
namespace {

TEST(Scale, NoDerivatives) {
  TextureCoordinates coordinates{Point(1.0, 2.0, 3.0),
                                 Vector(0.0, 0.0, 0.0),
                                 Vector(0.0, 0.0, 0.0),
                                 {2.0, 3.0},
                                 0.0,
                                 0.0,
                                 0.0,
                                 0.0};
  TextureCoordinates scaled = coordinates.Scale(0.5, 2.0, 1.0, 2.0);
  EXPECT_EQ(Point(1.0, 2.0, 3.0), scaled.p);
  EXPECT_EQ(Vector(0.0, 0.0, 0.0), scaled.dp_dx);
  EXPECT_EQ(Vector(0.0, 0.0, 0.0), scaled.dp_dy);
  EXPECT_EQ(2.0, scaled.uv[0]);
  EXPECT_EQ(8.0, scaled.uv[1]);
  EXPECT_EQ(0.0, scaled.du_dx);
  EXPECT_EQ(0.0, scaled.du_dy);
  EXPECT_EQ(0.0, scaled.dv_dx);
  EXPECT_EQ(0.0, scaled.dv_dy);
}

TEST(Scale, WithDerivatives) {
  TextureCoordinates coordinates{Point(1.0, 2.0, 3.0),
                                 Vector(1.0, 0.0, 0.0),
                                 Vector(0.0, 1.0, 0.0),
                                 {2.0, 3.0},
                                 2.0,
                                 3.0,
                                 4.0,
                                 5.0};
  TextureCoordinates scaled = coordinates.Scale(2.0, 4.0, 1.0, 2.0);
  EXPECT_EQ(Point(1.0, 2.0, 3.0), scaled.p);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), scaled.dp_dx);
  EXPECT_EQ(Vector(0.0, 1.0, 0.0), scaled.dp_dy);
  EXPECT_EQ(5.0, scaled.uv[0]);
  EXPECT_EQ(14.0, scaled.uv[1]);
  EXPECT_EQ(4.0, scaled.du_dx);
  EXPECT_EQ(6.0, scaled.du_dy);
  EXPECT_EQ(16.0, scaled.dv_dx);
  EXPECT_EQ(20.0, scaled.dv_dy);
}

}  // namespace
}  // namespace iris
