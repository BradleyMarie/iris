#include "iris/textures/scaled_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/textures/constant_texture.h"

TEST(ScaledValueTexture2DTest, Test) {
  auto texture0 =
      iris::MakeReferenceCounted<iris::textures::ConstantValueTexture2D<float>>(
          2.0);
  auto texture1 =
      iris::MakeReferenceCounted<iris::textures::ConstantValueTexture2D<float>>(
          4.0);
  iris::textures::ScaledValueTexture2D<float> scaled_texture(
      std::move(texture0), std::move(texture1));
  EXPECT_EQ(8.0, scaled_texture.Evaluate(iris::TextureCoordinates{}));
}