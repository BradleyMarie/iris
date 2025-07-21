#include "iris/textures/scaled_texture.h"

#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/textures/constant_texture.h"

namespace iris {
namespace textures {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetSpectralAllocator;
using ::testing::Return;

TEST(ScaledValueTexture2DTest, Make) {
  ReferenceCounted<ConstantValueTexture2D<float>> texture =
      MakeReferenceCounted<ConstantValueTexture2D<float>>(
          static_cast<float>(2.0));

  EXPECT_FALSE(MakeScaledValueTexture2D<float>(
      ReferenceCounted<ConstantValueTexture2D<float>>(),
      ReferenceCounted<ConstantValueTexture2D<float>>()));
  EXPECT_FALSE(MakeScaledValueTexture2D<float>(
      texture, ReferenceCounted<ConstantValueTexture2D<float>>()));
  EXPECT_FALSE(MakeScaledValueTexture2D<float>(
      ReferenceCounted<ConstantValueTexture2D<float>>(), texture));
  EXPECT_TRUE(MakeScaledValueTexture2D<float>(texture, texture));
}

TEST(ScaledValueTexture2DTest, Test) {
  ReferenceCounted<ConstantValueTexture2D<float>> texture0 =
      MakeReferenceCounted<ConstantValueTexture2D<float>>(
          static_cast<float>(2.0));
  ReferenceCounted<ConstantValueTexture2D<float>> texture1 =
      MakeReferenceCounted<ConstantValueTexture2D<float>>(
          static_cast<float>(4.0));

  ScaledValueTexture2D<float> scaled_texture(std::move(texture0),
                                             std::move(texture1));
  EXPECT_EQ(8.0, scaled_texture.Evaluate(
                     TextureCoordinates{{0.0, 0.0}, std::nullopt}));
}

TEST(ScaledSpectralTexture2DTest, Make) {
  ReferenceCounted<ConstantPointerTexture2D<Reflector, SpectralAllocator>>
      texture = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(
          MakeReferenceCounted<MockReflector>());

  EXPECT_FALSE(MakeScaledSpectralTexture2D<Reflector>(
      ReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(),
      ReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>()));
  EXPECT_FALSE(MakeScaledSpectralTexture2D<Reflector>(
      texture, ReferenceCounted<
                   ConstantPointerTexture2D<Reflector, SpectralAllocator>>()));
  EXPECT_FALSE(MakeScaledSpectralTexture2D<Reflector>(
      ReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(),
      texture));
  EXPECT_TRUE(MakeScaledSpectralTexture2D<Reflector>(texture, texture));
}

TEST(ScaledSpectralTexture2DTest, Test) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(1.0))
      .Times(2)
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  ReferenceCounted<ConstantPointerTexture2D<Reflector, SpectralAllocator>>
      texture = MakeReferenceCounted<
          ConstantPointerTexture2D<Reflector, SpectralAllocator>>(reflector);

  ScaledSpectralTexture2D<Reflector> scaled_texture(texture, texture);
  EXPECT_EQ(0.25, scaled_texture
                      .Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                GetSpectralAllocator())
                      ->Reflectance(1.0));
}

TEST(ScaledFloatTextureTest, Make) {
  ReferenceCounted<FloatTexture> texture = MakeConstantTexture(2.0);

  EXPECT_FALSE(MakeScaledTexture(ReferenceCounted<FloatTexture>(),
                                 ReferenceCounted<FloatTexture>()));
  EXPECT_FALSE(MakeScaledTexture(texture, ReferenceCounted<FloatTexture>()));
  EXPECT_FALSE(MakeScaledTexture(ReferenceCounted<FloatTexture>(), texture));
  EXPECT_TRUE(MakeScaledTexture(texture, texture));
}

TEST(ScaledFloatTextureTest, Test) {
  ReferenceCounted<FloatTexture> texture0 = MakeConstantTexture(2.0);
  ReferenceCounted<FloatTexture> texture1 = MakeConstantTexture(4.0);
  EXPECT_EQ(8.0, MakeScaledTexture(texture0, texture1)
                     ->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt}));
}

TEST(ScaledReflectorTextureTest, Make) {
  ReferenceCounted<ReflectorTexture> texture =
      MakeConstantTexture(MakeReferenceCounted<MockReflector>());

  EXPECT_FALSE(MakeScaledTexture(ReferenceCounted<ReflectorTexture>(),
                                 ReferenceCounted<ReflectorTexture>()));
  EXPECT_FALSE(
      MakeScaledTexture(texture, ReferenceCounted<ReflectorTexture>()));
  EXPECT_FALSE(
      MakeScaledTexture(ReferenceCounted<ReflectorTexture>(), texture));
  EXPECT_TRUE(MakeScaledTexture(texture, texture));
}

TEST(ScaledReflectorTextureTest, Test) {
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector, Reflectance(1.0))
      .Times(2)
      .WillRepeatedly(Return(static_cast<visual_t>(0.5)));

  ReferenceCounted<ReflectorTexture> texture = MakeConstantTexture(reflector);

  EXPECT_EQ(0.25, MakeScaledTexture(texture, texture)
                      ->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                 GetSpectralAllocator())
                      ->Reflectance(1.0));
}

}  // namespace
}  // namespace textures
}  // namespace iris
