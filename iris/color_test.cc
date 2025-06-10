#include "iris/color.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace {

TEST(ColorTest, Create) {
  Color color(1.0, 2.0, 3.0, Color::CIE_XYZ);
  EXPECT_EQ(1.0, color.x);
  EXPECT_EQ(2.0, color.y);
  EXPECT_EQ(3.0, color.z);
  EXPECT_EQ(Color::CIE_XYZ, color.space);
}

TEST(ColorTest, Equality) {
  Color c0(1.0, 2.0, 3.0, Color::CIE_XYZ);
  Color c1(2.0, 2.0, 3.0, Color::CIE_XYZ);
  Color c2(1.0, 3.0, 3.0, Color::CIE_XYZ);
  Color c3(1.0, 2.0, 4.0, Color::CIE_XYZ);
  Color c4(1.0, 2.0, 3.0, Color::LINEAR_SRGB);
  EXPECT_EQ(c0, c0);
  EXPECT_NE(c0, c1);
  EXPECT_NE(c0, c2);
  EXPECT_NE(c0, c3);
  EXPECT_NE(c0, c4);
}

TEST(ColorTest, ConvertToSame) {
  Color color(1.0, 2.0, 3.0, Color::CIE_XYZ);
  EXPECT_EQ(color, color.ConvertTo(Color::CIE_XYZ));
}

TEST(ColorTest, ConvertToLinearRgbFromXyz) {
  Color color(0.475235015, 0.50000006, 0.544414997, Color::CIE_XYZ);
  Color actual = color.ConvertTo(Color::LINEAR_SRGB);
  EXPECT_EQ(Color::LINEAR_SRGB, actual.space);
  EXPECT_NEAR(0.5, actual.r, 0.001);
  EXPECT_NEAR(0.5, actual.g, 0.001);
  EXPECT_NEAR(0.5, actual.b, 0.001);
}

TEST(ColorTest, ConvertToXyzFromLinearRgb) {
  Color color(0.5, 0.5, 0.5, Color::LINEAR_SRGB);
  Color actual = color.ConvertTo(Color::CIE_XYZ);
  EXPECT_EQ(Color::CIE_XYZ, actual.space);
  EXPECT_NEAR(0.475235015, actual.r, 0.001);
  EXPECT_NEAR(0.50000006, actual.g, 0.001);
  EXPECT_NEAR(0.544414997, actual.b, 0.001);
}

TEST(ColorTest, LumaSame) {
  Color color(1.0, 2.0, 3.0, Color::CIE_XYZ);
  EXPECT_EQ(color.y, color.Luma());
}

TEST(ColorTest, LumaFromLinearRgb) {
  Color color(0.5, 0.5, 0.5, Color::LINEAR_SRGB);
  EXPECT_NEAR(0.50000006, color.Luma(), 0.001);
}

}  // namespace
}  // namespace iris
