#include "iris/textures/scaled_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/textures/constant_texture.h"

TEST(Scale, NoDerivatives) {
  iris::TextureCoordinates coordinates{{2.0, 3.0}};
  auto scaled = iris::textures::internal::Scale(coordinates, 0.5, 2.0);
  EXPECT_EQ(1.0, scaled.uv[0]);
  EXPECT_EQ(6.0, scaled.uv[1]);
  EXPECT_FALSE(scaled.derivatives.has_value());
}

TEST(Scale, WithDerivatives) {
  iris::TextureCoordinates coordinates{
      {2.0, 3.0},
      {{2.0, 3.0, 4.0, 5.0, iris::Vector(2.0, 4.0, 8.0),
        iris::Vector(16.0, 32.0, 64.0)}}};
  auto scaled = iris::textures::internal::Scale(coordinates, 2.0, 4.0);
  EXPECT_EQ(4.0, scaled.uv[0]);
  EXPECT_EQ(12.0, scaled.uv[1]);
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

TEST(ScaledValueTexture2DTest, Test) {
  auto texture =
      iris::MakeReferenceCounted<iris::textures::ConstantValueTexture2D<float>>(
          1.0);
  iris::textures::ScaledValueTexture2D<float> scaled_texture(texture, 1.0, 1.0);
  EXPECT_EQ(1.0, scaled_texture.Evaluate(iris::TextureCoordinates{}));
}

TEST(PointerValueTexture2DTest, Test) {
  auto value = iris::MakeReferenceCounted<iris::spectra::MockSpectrum>();
  auto texture = iris::MakeReferenceCounted<
      iris::textures::ConstantPointerTexture2D<iris::Spectrum>>(value);
  iris::textures::ScaledPointerTexture2D<iris::Spectrum> scaled_texture(
      texture, 1.0, 1.0);
  EXPECT_EQ(value.Get(), scaled_texture.Evaluate(iris::TextureCoordinates{}));
}