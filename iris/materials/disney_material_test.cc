#include "iris/materials/disney_material.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/point.h"
#include "iris/reflectors/mock_reflector.h"
#include "iris/testing/bxdf_allocator.h"
#include "iris/testing/spectral_allocator.h"
#include "iris/textures/constant_texture.h"
#include "iris/textures/float_texture.h"
#include "iris/textures/reflector_texture.h"

namespace iris {
namespace materials {
namespace {

using ::iris::reflectors::MockReflector;
using ::iris::testing::GetBxdfAllocator;
using ::iris::testing::GetSpectralAllocator;
using ::iris::textures::FloatTexture;
using ::iris::textures::MakeConstantTexture;
using ::testing::_;
using ::testing::Return;

TEST(DisneyMaterialTest, NullMaterial) {
  ReferenceCounted<FloatTexture> float_texture = MakeConstantTexture(0.0);
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();

  EXPECT_FALSE(MakeDisneyMaterial(
      MakeConstantTexture(reflector), float_texture,
      ReferenceCounted<FloatTexture>(), float_texture, float_texture,
      float_texture, float_texture, float_texture, float_texture, float_texture,
      float_texture, float_texture, float_texture, float_texture,
      MakeConstantTexture(reflector), false));

  EXPECT_FALSE(MakeDisneyMaterial(
      MakeConstantTexture(reflector), float_texture, float_texture,
      ReferenceCounted<FloatTexture>(), float_texture, float_texture,
      float_texture, float_texture, float_texture, float_texture, float_texture,
      float_texture, float_texture, float_texture,
      MakeConstantTexture(reflector), false));
}

TEST(DisneyMaterialTest, Evaluate) {
  ReferenceCounted<FloatTexture> float_texture = MakeConstantTexture(1.0);
  ReferenceCounted<MockReflector> reflector =
      MakeReferenceCounted<MockReflector>();

  ReferenceCounted<Material> material = MakeDisneyMaterial(
      MakeConstantTexture(reflector), float_texture, float_texture,
      float_texture, float_texture, float_texture, float_texture, float_texture,
      float_texture, float_texture, float_texture, float_texture, float_texture,
      float_texture, MakeConstantTexture(reflector), false);
  ASSERT_TRUE(material);

  EXPECT_TRUE(material->Evaluate(TextureCoordinates{Point(0.0, 0.0, 0.0),
                                                    Vector(0.0, 0.0, 0.0),
                                                    Vector(0.0, 0.0, 0.0),
                                                    {0.0, 0.0},
                                                    0.0,
                                                    0.0,
                                                    0.0,
                                                    0.0},
                                 GetSpectralAllocator(), GetBxdfAllocator()));
}

}  // namespace
}  // namespace materials
}  // namespace iris
