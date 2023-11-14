#include "iris/textures/scaled_texture.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"

TEST(ScaledValueTexture2DTest, Test) {
  auto texture0 =
      iris::MakeReferenceCounted<iris::textures::ConstantValueTexture2D<float>>(
          static_cast<float>(2.0));
  auto texture1 =
      iris::MakeReferenceCounted<iris::textures::ConstantValueTexture2D<float>>(
          static_cast<float>(4.0));
  iris::textures::ScaledValueTexture2D<float> scaled_texture(
      std::move(texture0), std::move(texture1));
  EXPECT_EQ(8.0, scaled_texture.Evaluate(
                     iris::TextureCoordinates{{0.0, 0.0}, std::nullopt}));
}

TEST(ScaledSpectralTexture2DTest, Test) {
  auto reflector =
      iris::MakeReferenceCounted<iris::reflectors::MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(1.0))
      .Times(2)
      .WillRepeatedly(testing::Return(static_cast<iris::visual_t>(0.5)));
  auto texture =
      iris::MakeReferenceCounted<iris::textures::ConstantPointerTexture2D<
          iris::Reflector, iris::SpectralAllocator>>(reflector);
  iris::textures::ScaledSpectralTexture2D<iris::Reflector> scaled_texture(
      texture, texture);
  EXPECT_EQ(0.25,
            scaled_texture
                .Evaluate(iris::TextureCoordinates{{0.0, 0.0}, std::nullopt},
                          iris::testing::GetSpectralAllocator())
                ->Reflectance(1.0));
}