#include "iris/texture_coordinates.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {
namespace {

TEST(TextureCoordinates, Offset) {
  TextureCoordinates coordinates{Point(1.0, 2.0, 3.0),
                                 Vector(1.0, 0.0, 0.0),
                                 Vector(0.0, 1.0, 0.0),
                                 {2.0, 3.0},
                                 2.0,
                                 3.0,
                                 4.0,
                                 5.0};
  TextureCoordinates offset =
      coordinates.Offset(Vector(1.0, 2.0, 3.0), 2.0, 4.0);
  EXPECT_EQ(Point(2.0, 4.0, 6.0), offset.p);
  EXPECT_EQ(Vector(1.0, 0.0, 0.0), offset.dp_dx);
  EXPECT_EQ(Vector(0.0, 1.0, 0.0), offset.dp_dy);
  EXPECT_EQ(4.0, offset.uv[0]);
  EXPECT_EQ(7.0, offset.uv[1]);
  EXPECT_EQ(2.0, offset.du_dx);
  EXPECT_EQ(3.0, offset.du_dy);
  EXPECT_EQ(4.0, offset.dv_dx);
  EXPECT_EQ(5.0, offset.dv_dy);
}

TEST(TextureCoordinates, Scale) {
  TextureCoordinates coordinates{Point(1.0, 2.0, 3.0),
                                 Vector(0.0, 0.0, 0.0),
                                 Vector(0.0, 0.0, 0.0),
                                 {2.0, 3.0},
                                 0.0,
                                 0.0,
                                 0.0,
                                 0.0};
  TextureCoordinates scaled = coordinates.Scale(0.5, 2.0);
  EXPECT_EQ(Point(1.0, 2.0, 3.0), scaled.p);
  EXPECT_EQ(Vector(0.0, 0.0, 0.0), scaled.dp_dx);
  EXPECT_EQ(Vector(0.0, 0.0, 0.0), scaled.dp_dy);
  EXPECT_EQ(1.0, scaled.uv[0]);
  EXPECT_EQ(6.0, scaled.uv[1]);
  EXPECT_EQ(0.0, scaled.du_dx);
  EXPECT_EQ(0.0, scaled.du_dy);
  EXPECT_EQ(0.0, scaled.dv_dx);
  EXPECT_EQ(0.0, scaled.dv_dy);
}

}  // namespace
}  // namespace iris
