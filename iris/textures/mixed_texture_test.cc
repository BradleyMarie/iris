#include "iris/textures/mixed_texture.h"

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

TEST(ScaledFloatTextureTest, Make) {
  ReferenceCounted<FloatTexture> texture = MakeConstantTexture(2.0);

  EXPECT_FALSE(MakeMixedTexture(ReferenceCounted<FloatTexture>(),
                                ReferenceCounted<FloatTexture>(), texture));
  EXPECT_FALSE(MakeMixedTexture(ReferenceCounted<FloatTexture>(), texture,
                                ReferenceCounted<FloatTexture>()));
  EXPECT_TRUE(MakeMixedTexture(texture, texture, texture));
}

TEST(ScaledFloatTextureTest, Test) {
  ReferenceCounted<FloatTexture> texture0 = MakeConstantTexture(2.0);
  ReferenceCounted<FloatTexture> texture1 = MakeConstantTexture(4.0);
  ReferenceCounted<FloatTexture> amount = MakeConstantTexture(0.5);
  EXPECT_EQ(3.0, MakeMixedTexture(texture0, texture1, amount)
                     ->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt}));
}

TEST(ScaledReflectorTextureTest, Make) {
  ReferenceCounted<ReflectorTexture> reflector_texture =
      MakeConstantTexture(MakeReferenceCounted<MockReflector>());
  ReferenceCounted<FloatTexture> float_texture = MakeConstantTexture(2.0);

  EXPECT_FALSE(MakeMixedTexture(ReferenceCounted<ReflectorTexture>(),
                                ReferenceCounted<ReflectorTexture>(),
                                float_texture));
  EXPECT_FALSE(MakeMixedTexture(ReferenceCounted<ReflectorTexture>(),
                                reflector_texture,
                                ReferenceCounted<FloatTexture>()));
  EXPECT_TRUE(
      MakeMixedTexture(reflector_texture, reflector_texture, float_texture));
}

TEST(ScaledReflectorTextureTest, Test) {
  ReferenceCounted<MockReflector> reflector0 =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector0, Reflectance(1.0))
      .WillOnce(Return(static_cast<visual_t>(0.0)));

  ReferenceCounted<MockReflector> reflector1 =
      MakeReferenceCounted<MockReflector>();
  EXPECT_CALL(*reflector1, Reflectance(1.0))
      .WillOnce(Return(static_cast<visual_t>(0.5)));

  ReferenceCounted<ReflectorTexture> texture0 = MakeConstantTexture(reflector0);
  ReferenceCounted<ReflectorTexture> texture1 = MakeConstantTexture(reflector1);
  ReferenceCounted<FloatTexture> amount = MakeConstantTexture(0.5);

  EXPECT_EQ(0.25, MakeMixedTexture(texture0, texture1, amount)
                      ->Evaluate(TextureCoordinates{{0.0, 0.0}, std::nullopt},
                                 GetSpectralAllocator())
                      ->Reflectance(1.0));
}

}  // namespace
}  // namespace textures
}  // namespace iris
